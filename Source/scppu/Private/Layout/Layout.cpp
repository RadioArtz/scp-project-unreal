

#include "Layout/Layout.h"
#include "Layout/LayoutCell.h"
#include "Layout/BaseLayoutSpawnValidator.h"
#include "Engine/DataTable.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Components/BillboardComponent.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY(LogLayout);

// Sets default values
ALayout::ALayout()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	this->PrimaryActorTick.bCanEverTick = false;

	// Root component
	this->Root = CreateDefaultSubobject<USceneComponent >("Root", false);
	this->Root->SetMobility(EComponentMobility::Static);
	this->SetRootComponent(this->Root);

#if WITH_EDITOR
	// Sprite component
	this->Sprite = CreateDefaultSubobject<UBillboardComponent>("Sprite", false);
	this->Sprite->SetSprite(ConstructorHelpers::FObjectFinder<UTexture2D>(TEXT("Texture2D'/Engine/EditorResources/EmptyActor.EmptyActor'")).Object);
	this->Sprite->bIsScreenSizeScaled = true;
	this->Sprite->ScreenSize = 0.0025f;
	this->Sprite->SetRelativeLocation(FVector(0, 0, 0));
	this->Sprite->AttachToComponent(this->Root, FAttachmentTransformRules::KeepRelativeTransform);
#endif
}

bool ALayout::SetConfig(FIntVector2 NewGridSize, float NewCellSize, UDataTable* NewDataTable)
{
	if (this->bIsLayoutPresent)
	{
		return false;
	}

	this->GridSize = NewGridSize;
	this->CellSize = NewCellSize;
	this->DataTable = NewDataTable;
	return true;
}

bool ALayout::Initialize(int32 NewSeed)
{
	if (!IsValid(this->DataTable))
	{
		UE_LOG(LogLayout, Error, TEXT("%s: Not able to create new layout, data table reference is not valid"), *this->GetName());
		return false;
	}

	if (this->DataTable->RowStruct != FLayoutCellGenerationSettings::StaticStruct())
	{
#if WITH_EDITORONLY_DATA
		UE_LOG(LogLayout, Error, TEXT("%s: Not able to create new layout, data table contains wrong struct type (is '%s', should be '%s')"), *this->GetName(), *this->DataTable->RowStructName.ToString(), TEXT("FLayoutCellGenerationSettings"));
#else
		UE_LOG(LogLayout, Error, TEXT("%s: Not able to create new layout, data table contains wrong struct type"), *this->GetName());
#endif
		return false;
	}

	for (auto& Kvp : this->DataTable->GetRowMap())
	{
		FLayoutCellGenerationSettings* Row = (FLayoutCellGenerationSettings*)Kvp.Value;

		if (!Row->PassedSanityCheck())
		{
			UE_LOG(LogLayout, Error, TEXT("%s: Not able to create new layout, row '%s' did not pass sanity check"), *this->GetName(), *Kvp.Key.ToString());
			return false;
		}
	}

	if (this->bIsLayoutPresent)
	{
		UE_LOG(LogLayout, Warning, TEXT("%s: Not able to create new layout, layout already present"), *this->GetName());
		return false;
	}

	if (this->bIsReadOnly)
	{
		UE_LOG(LogLayout, Error, TEXT("%s: Not able to create new layout, currently read-only"), *this->GetName());
		return false;
	}

	bIsLayoutPresent = true;
	this->RStream = FRandomStream(NewSeed);
	this->Grid.Empty(this->GridSize.X * this->GridSize.Y);
	for (int x = 0; x < this->GridSize.X; x++)
	{
		for (int y = 0; y < this->GridSize.Y; y++)
		{
			ULayoutCell* Cell = NewObject<ULayoutCell>(this, ULayoutCell::StaticClass(), FName(FString::Printf(TEXT("%s_Cell_X%d_Y%d"), *this->GetName(), x, y)));
			Cell->Location = FIntVector2(x, y);
			Cell->Rotation = 0;
			Cell->UniqueSeed = RStream.RandRange(0, MAX_int32 - 1);
			Grid.Add(FIntVector2(x, y), Cell);
		}
	}

	UE_LOG(LogLayout, Log, TEXT("%s: Successfully created new layout"), *this->GetName());
	return true;
}

bool ALayout::Clear()
{
	if (!this->bIsLayoutPresent)
	{
		UE_LOG(LogLayout, Warning, TEXT("%s: Not able to clear layout, no layout present"), *this->GetName());
		return false;
	}

	if (this->bIsReadOnly)
	{
		UE_LOG(LogLayout, Error, TEXT("%s: Not able to clear layout, currently read-only"), *this->GetName());
		return false;
	}

	for (auto Kvp : this->Grid)
	{
		Kvp.Value->UnloadSublevel(true);
		Kvp.Value->ConditionalBeginDestroy();
	}

	this->Grid.Empty();
	bIsLayoutPresent = false;
	UE_LOG(LogLayout, Log, TEXT("%s: Successfully cleared layout"), *this->GetName());
	return true;
}

ULayoutCell* ALayout::GetCellFromWorldLocation(FVector WorldLocation, float ZTolerance)
{
	if (ZTolerance >= 0 && (this->GetActorLocation().Z > WorldLocation.Z + ZTolerance || this->GetActorLocation().Z < WorldLocation.Z - ZTolerance))
	{
		return nullptr;
	}

	FVector RelativeLocation = WorldLocation - this->GetActorLocation() - FVector(this->CellSize / 2, this->CellSize / 2, 0);
	FIntVector2 CellLocation = FIntVector2(FMath::DivideAndRoundNearest(RelativeLocation.X, this->CellSize), FMath::DivideAndRoundNearest(RelativeLocation.Y, this->CellSize));
	return this->GetCell(CellLocation);
}

void ALayout::GetNeighbouringCells(ULayoutCell* Origin, bool bOnlyReturnConnectedCells, ULayoutCell*& OutCellPX, ULayoutCell*& OutCellPY, ULayoutCell*& OutCellNX, ULayoutCell*& OutCellNY)
{
	OutCellPX = this->GetCell(FIntVector2(Origin->Location.X + 1, Origin->Location.Y));
	OutCellPY = this->GetCell(FIntVector2(Origin->Location.X, Origin->Location.Y + 1));
	OutCellNX = this->GetCell(FIntVector2(Origin->Location.X - 1, Origin->Location.Y));
	OutCellNY = this->GetCell(FIntVector2(Origin->Location.X, Origin->Location.Y - 1));

	if (bOnlyReturnConnectedCells)
	{
		OutCellPX = (Origin->HasConnections.bPX) ? OutCellPX : nullptr;
		OutCellPY = (Origin->HasConnections.bPY) ? OutCellPY : nullptr;
		OutCellNX = (Origin->HasConnections.bNX) ? OutCellNX : nullptr;
		OutCellNY = (Origin->HasConnections.bNY) ? OutCellNY : nullptr;
	}
}

void ALayout::FindCellsWithRowName(FName RowName, TArray<ULayoutCell*>& OutCells)
{
	for (auto Kvp : this->Grid)
	{
		if (Kvp.Value->RowName == RowName)
		{
			OutCells.Add(Kvp.Value);
		}
	}
}

bool ALayout::DoesPathExist(ULayoutCell* Start, ULayoutCell* Goal)
{
	if (!IsValid(Start) || !IsValid(Goal))
	{
		return false;
	}

	ULayoutCell* CellPX;
	ULayoutCell* CellPY;
	ULayoutCell* CellNX;
	ULayoutCell* CellNY;
	TArray<ULayoutCell*> Queue;
	Queue.AddUnique(Start);

	for (int i = 0; i < Queue.Num(); i++)
	{
		if (Queue[i] == Goal)
		{
			return true;
		}

		this->GetNeighbouringCells(Queue[i], true, CellPX, CellPY, CellNX, CellNY);
		Queue.AddUnique(CellPX);
		Queue.AddUnique(CellPY);
		Queue.AddUnique(CellNX);
		Queue.AddUnique(CellNY);
	}
	
	return false;
}

void ALayout::LoadAllSublevels(bool bShowAllSublevels)
{
	for (auto Kvp : this->Grid)
	{
		Kvp.Value->LoadSublevel(bShowAllSublevels);
	}
}

void ALayout::UnloadAllSublevels()
{
	for (auto Kvp : this->Grid)
	{
		Kvp.Value->UnloadSublevel(true);
	}
}

void ALayout::DrawDebug(float Duration, bool bDrawCells, bool bShowText) //Change this into a switchable debug command somehow
{
	if (Duration < 0.0)
	{
		Duration = 1e+18;
	}

	for (auto Kvp : this->Grid)
	{
		DrawDebugBox(this->GetWorld(), Kvp.Value->GetWorldLocation(), FVector(this->CellSize * 0.5f, this->CellSize * 0.5f, 0), FColor::Blue, false, Duration, 0, 20.0f);

		if (bDrawCells)
		{
			Kvp.Value->DrawDebug(Duration, bShowText);
		}
	}
}

// Called when the game starts or when spawned
void ALayout::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogLayout, Log, TEXT("%s: Disabling 'MaxDebugStringsPerActor' so debug draw will work properly"), *this->GetName());
	GEngine->Exec(this->GetWorld(), TEXT("r.DebugSafeZone.MaxDebugTextStringsPerActor 0"));
}

// Called every frame
void ALayout::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

