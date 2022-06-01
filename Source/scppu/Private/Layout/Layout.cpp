

#include "Layout/Layout.h"
#include "Layout/LayoutCell.h"
#include "Engine/DataTable.h"

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

bool ALayout::NewLayout(FIntVector2 NewGridSize, float NewCellSize, int32 NewSeed)
{
	if (this->DataTable == nullptr)
	{
		UE_LOG(LogLayout, Warning, TEXT("%s: Not able to create new layout, data table reference returned nullptr"), *this->GetName());
		return false;
	}

	if (this->DataTable->RowStruct != FLayoutCellGenerationSettings::StaticStruct())
	{
		UE_LOG(LogLayout, Warning, TEXT("%s: Not able to create new layout, data table contains wrong struct type (%s)"), *this->GetName(), *this->DataTable->RowStructName.ToString())
		return false;
	}

	if (this->bIsLayoutPresent)
	{
		UE_LOG(LogLayout, Warning, TEXT("%s: Not able to create new layout, layout already present"), *this->GetName());
		return false;
	}

	bIsLayoutPresent = true;
	this->GridSize = NewGridSize;
	this->CellSize = CellSize;
	this->RandStream = FRandomStream(NewSeed);
	this->Grid.Empty(this->GridSize.X * this->GridSize.Y);
	for (int x = 0; x < this->GridSize.X; x++)
	{
		for (int y = 0; y < this->GridSize.Y; y++)
		{
			ULayoutCell* Cell = NewObject<ULayoutCell>(this, ULayoutCell::StaticClass());
			Cell->Location = FIntVector2(x, y);
			Cell->Rotation = 0;
			Cell->Owner = this;
			Cell->RandStream = FRandomStream(RandStream.RandHelper(MAX_int32));
			Cell->UniqueSublevelName = FString::Printf(TEXT("%s__X%d_Y%d"), *this->GetName(), x, y);
			Grid.Add(FIntVector2(x, y), Cell);
		}
	}

	UE_LOG(LogLayout, Log, TEXT("%s: Successfully created new layout"), *this->GetName());
	return true;
}

bool ALayout::ClearLayout()
{
	if (!this->bIsLayoutPresent)
	{
		UE_LOG(LogLayout, Warning, TEXT("%s: Not able to clear layout, no layout present"), *this->GetName());
		return false;
	}

	bIsLayoutPresent = false;
	for (auto Kvp : this->Grid)
	{
		Kvp.Value->UnloadSublevel();
		Kvp.Value->ConditionalBeginDestroy();
	}

	this->Grid.Empty();
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
	//later
}

bool ALayout::DoesPathExist(ULayoutCell* Start, ULayoutCell* Target)
{
	// later
	return false;
}

void ALayout::LoadAllSublevels()
{
	for (auto Kvp : Grid)
	{
		Kvp.Value->LoadSublevel();
	}
}

void ALayout::UnloadAllSublevels()
{
	for (auto Kvp : Grid)
	{
		Kvp.Value->UnloadSublevel();
	}
}

void ALayout::DrawDebug(float Duration, bool bDrawCellContent)
{
	//later
}

// Called when the game starts or when spawned
void ALayout::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALayout::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

