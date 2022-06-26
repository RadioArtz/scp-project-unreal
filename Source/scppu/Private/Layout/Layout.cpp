

#include "Layout/Layout.h"
#include "Layout/LayoutCell.h"
#include "Layout/LayoutSpawnValidator.h"
#include "Engine/DataTable.h"
#include "Engine/LevelStreamingDynamic.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY(LogLayout);

// Sets default values
ALayout::ALayout()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Add a mesh component and set it as scene root for easier in editor use.
	UStaticMesh* MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'")).Object;
	UMaterialInterface* MeshMaterial = ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("Material'/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial'")).Object;
	UStaticMeshComponent* MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Debug Mesh", false);
	MeshComponent->SetStaticMesh(MeshAsset);
	MeshComponent->SetMaterial(0, MeshMaterial);
	MeshComponent->bHiddenInGame = true;
	SetRootComponent(MeshComponent);
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
			Cell->Owner = this;
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

	for (auto Kvp : this->Grid)
	{
		Kvp.Value->UnloadSublevel();
		Kvp.Value->ConditionalBeginDestroy();
	}

	this->Grid.Empty();
	bIsLayoutPresent = false;
	UE_LOG(LogLayout, Log, TEXT("%s: Successfully cleared layout"), *this->GetName());
	return true;
}

ULayoutCell* ALayout::GetCell(FIntVector2 Location)
{
	if (this->Grid.Find(Location) == nullptr)
	{
		return nullptr;
	}

	return Grid[Location];
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

void ALayout::GetNeighbouringCells(ULayoutCell* Origin, bool bOnlyReturnConnectedCells, ULayoutCell*& OutCellPX, ULayoutCell*& OutCellPY, ULayoutCell*& OutCellNX, ULayoutCell*& OutCellNY)
{
	OutCellPX = GetCell(FIntVector2(Origin->Location.X + 1, Origin->Location.Y));
	OutCellPY = GetCell(FIntVector2(Origin->Location.X, Origin->Location.Y + 1));
	OutCellNX = GetCell(FIntVector2(Origin->Location.X - 1, Origin->Location.Y));
	OutCellNY = GetCell(FIntVector2(Origin->Location.X, Origin->Location.Y - 1));

	if (bOnlyReturnConnectedCells)
	{
		OutCellPX = (Origin->HasConnections.bPX) ? OutCellPX : nullptr;
		OutCellPY = (Origin->HasConnections.bPY) ? OutCellPY : nullptr;
		OutCellNX = (Origin->HasConnections.bNX) ? OutCellNX : nullptr;
		OutCellNY = (Origin->HasConnections.bNY) ? OutCellNY : nullptr;
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
		Kvp.Value->LoadSublevel();
		
		if (IsValid(Kvp.Value->Sublevel))
		{
			Kvp.Value->Sublevel->SetShouldBeVisible(bShowAllSublevels);
		}
	}
}

void ALayout::UnloadAllSublevels()
{
	for (auto Kvp : this->Grid)
	{
		Kvp.Value->UnloadSublevel();
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

