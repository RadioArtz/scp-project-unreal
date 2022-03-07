

#include "LayoutGenerator_Main.h"
#include "Components/StaticMeshComponent.h" 
#include "Engine/LevelStreamingDynamic.h" 
#include "DrawDebugHelpers.h" 
#include "LayoutGenerator_SpawnValidator.h"
#include "LayoutGenerator_Cell.h"

DEFINE_LOG_CATEGORY(LogLayoutGenerator);

// Sets default values
ALayoutGenerator_Main::ALayoutGenerator_Main()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Add a mesh component and set it as scene root for easier in editor use.
	UStaticMesh* MeshAsset;
	UMaterialInterface* MeshMaterial;
	UStaticMeshComponent* MeshComponent;

	MeshAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'")).Object;
	MeshMaterial = ConstructorHelpers::FObjectFinder<UMaterialInterface>(TEXT("Material'/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial'")).Object;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Debug Mesh", false);

	MeshComponent->SetStaticMesh(MeshAsset);
	MeshComponent->SetMaterial(0, MeshMaterial);
	MeshComponent->bHiddenInGame = true;
	SetRootComponent(MeshComponent);
}

void ALayoutGenerator_Main::AsyncGenerateLayout(int32 NewSeed, bool bShowAllLevelsWhenDone)
{
	bool bSuccess;
	ELayoutGeneratorErrors OutError;

	UE_LOG(LogLayoutGenerator, Display, TEXT("%s: Starting layout generation..."), *GetName());
	bSuccess = true;
	OutError = ELayoutGeneratorErrors::LGE_Unknown;

	// Don't generate if we are currently generating or if a layout is already present
	if (bIsCurrentlyGeneratingLayout || bIsLayoutPresent)
	{
		bSuccess = false;
		OutError = ELayoutGeneratorErrors::LGE_LayoutAlreadyPresent;

		OnLayoutGenerated.Broadcast(false, OutError);
		UE_LOG(LogLayoutGenerator, Error, TEXT("%s: Failed to generate layout. Error code: %i (%s)"), *GetName(), (uint8)OutError, *UEnum::GetValueAsName(OutError).ToString());
		return;
	}

	ResetRuntimeProperties();
	bIsCurrentlyGeneratingLayout = true;
	Seed = NewSeed;

	OnLayoutGenerationStatusUpdate.Broadcast(.0f, ELayoutGeneratorTasks::LGT_InitProperties);
	bSuccess = bSuccess && InitRuntimeProperties(OutError);

	// Do the calculations on a diffrent thread, so we don't freeze the game thread
	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this, bShowAllLevelsWhenDone, bSuccess, OutError]() mutable
	{
		OnLayoutGenerationStatusUpdate.Broadcast(.1f, ELayoutGeneratorTasks::LGT_GeneratingRooms);
		bSuccess = bSuccess && GenerateRequiredRooms(OutError);
		bSuccess = bSuccess && GenerateFromQueue(OutError);
		OnLayoutGenerationStatusUpdate.Broadcast(.6f, ELayoutGeneratorTasks::LGT_RunningPostSpawnValidation);
		bSuccess = bSuccess && RunPostSpawnValidation(OutError);

		if (!bSuccess)
		{
			ResetRuntimeProperties();
			bIsCurrentlyGeneratingLayout = false;
			bIsLayoutPresent = false;

			OnLayoutGenerated.Broadcast(false, OutError);
			UE_LOG(LogLayoutGenerator, Error, TEXT("%s: Failed to generate layout. Error code: %i (%s)"), *GetName(), (uint8)OutError, *UEnum::GetValueAsName(OutError).ToString());
			return;
		}

		// Load levels on the game thread, because ue4 wants us to do that
		// From here on, nothing can fail anymore
		AsyncTask(ENamedThreads::GameThread, [this, bShowAllLevelsWhenDone]()
		{
			OnLayoutGenerationStatusUpdate.Broadcast(.7f, ELayoutGeneratorTasks::LGT_LoadingLevels);

			for (auto Kvp : Grid)
			{
				Kvp.Value->LoadLevel();
				if (Kvp.Value->LevelStreamingDynamic != nullptr)
				{
					Kvp.Value->LevelStreamingDynamic->SetShouldBeVisible(bShowAllLevelsWhenDone);
					Kvp.Value->LevelStreamingDynamic->OnLevelLoaded.AddDynamic(this, &ALayoutGenerator_Main::OnLevelLoadedCallback);
					LevelsCurrentlyLoading++;
				};

				/** ALayoutGenerator_Main::OnLevelLoadedCallback completes generation */
			}
		});
	});

	return;
}

bool ALayoutGenerator_Main::ClearLayout()
{
	if (bIsCurrentlyGeneratingLayout || !bIsLayoutPresent)
	{
		OnLayoutCleared.Broadcast(false, ELayoutGeneratorErrors::LGE_NoLayoutPresent);
		return false;
	}

	ResetRuntimeProperties();
	bIsLayoutPresent = false;
	OnLayoutCleared.Broadcast(true, ELayoutGeneratorErrors::LGE_None);

	return true;
}

TArray<FIntVector2D> ALayoutGenerator_Main::FindCellLocationsWithRoomRowName(FName RoomRowName)
{
	TArray<FIntVector2D> FoundCellLocations;

	for (auto Kvp : Grid)
	{
		if (Kvp.Value->RoomRowName == RoomRowName)
		{
			FoundCellLocations.AddUnique(Kvp.Key);
		}
	}

	return FoundCellLocations;
}

ULayoutGenerator_Cell* ALayoutGenerator_Main::GetCell(FIntVector2D Location)
{
	if (Grid.Find(Location) != nullptr)
	{
		return Grid[Location];
	}
	else
	{
		return nullptr;
	}
}

bool ALayoutGenerator_Main::DoesPathExist(ULayoutGenerator_Cell* StartingCell, ULayoutGenerator_Cell* EndingCell)
{
	if (StartingCell == nullptr || EndingCell == nullptr)
	{
		return false;
	}

	bool bSuccess;
	int CurrentIteration;
	int MaxIteration;
	TArray<ULayoutGenerator_Cell*> CellQueue;
	TArray<ULayoutGenerator_Cell*> CellsDone;

	bSuccess = false;
	CurrentIteration = 0;
	MaxIteration = GridSize.X * GridSize.Y;
	CellQueue.Add(StartingCell);

	while (CellQueue.Num() > 0)
	{
		ULayoutGenerator_Cell* ThisCell;
		ULayoutGenerator_Cell* PosXCell;
		ULayoutGenerator_Cell* PosYCell;
		ULayoutGenerator_Cell* NegXCell;
		ULayoutGenerator_Cell* NegYCell;

		ThisCell = CellQueue[0];
		CellQueue.Remove(ThisCell);
		CellsDone.Add(ThisCell);
		CurrentIteration++;

		// We've hit the max iteration limit
		if (CurrentIteration > MaxIteration)
		{
			bSuccess = false;
			break;
		}

		// We found the cell we are searching for
		if (ThisCell == EndingCell)
		{
			bSuccess = true;
			break;
		}

		bSuccess = false;
		PosXCell = GetCell(FIntVector2D(ThisCell->Location.X + 1, ThisCell->Location.Y));
		PosYCell = GetCell(FIntVector2D(ThisCell->Location.X, ThisCell->Location.Y + 1));
		NegXCell = GetCell(FIntVector2D(ThisCell->Location.X - 1, ThisCell->Location.Y));
		NegYCell = GetCell(FIntVector2D(ThisCell->Location.X, ThisCell->Location.Y - 1));

		// Add connecting neighbours who were not already added prior to the queue //
		if (PosXCell != nullptr && !CellsDone.Contains(PosXCell) && ThisCell->HasDoor.bPositiveX)
		{
			CellQueue.Add(PosXCell);
		}

		if (PosYCell != nullptr && !CellsDone.Contains(PosYCell) && ThisCell->HasDoor.bPositiveY)
		{
			CellQueue.Add(PosYCell);
		}

		if (NegXCell != nullptr && !CellsDone.Contains(NegXCell) && ThisCell->HasDoor.bNegativeX)
		{
			CellQueue.Add(NegXCell);
		}

		if (NegYCell != nullptr && !CellsDone.Contains(NegYCell) && ThisCell->HasDoor.bNegativeY)
		{
			CellQueue.Add(NegYCell);
		}
		////
	};

	return bSuccess;
}

void ALayoutGenerator_Main::DrawDebug(float Duration, bool bDrawCells)
{
	// Draw grid //
	for (auto Kvp : Grid)
	{
		DrawDebugBox(GetWorld(), Kvp.Value->GetWorldLocation(), FVector(CellSize * .5f, CellSize * .5f, 0.f), FColor::Blue, false, Duration, 0, 20.f);
	}
	////

	// Draw cell //
	if (bDrawCells)
	{
		GEngine->Exec(GetWorld(), TEXT("r.DebugSafeZone.MaxDebugTextStringsPerActor 0"));

		for (auto Kvp : Grid)
		{
			Kvp.Value->DrawDebug(Duration);
		}
	}
	////
}

// Called when the game starts or when spawned
void ALayoutGenerator_Main::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALayoutGenerator_Main::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALayoutGenerator_Main::EndPlay()
{
	ClearLayout();
}

bool ALayoutGenerator_Main::InitRuntimeProperties(ELayoutGeneratorErrors& OutError)
{
	// DataTable initialisation //
	if (DataTable == nullptr)
	{
		OutError = ELayoutGeneratorErrors::LGE_NoDatatable;
		return false;
	}

	if (DataTable->RowStruct != FRoomGenerationSettings::StaticStruct())
	{
		OutError = ELayoutGeneratorErrors::LGE_WrongDatatableStruct;
		return false;
	}

	for (auto Kvp : DataTable->GetRowMap())
	{
		RoomGenerationData.Add(Kvp.Key, *(FRoomGenerationSettings*)Kvp.Value);
	}
	////

	// Properties initialisation //
	RandomStream = FRandomStream(Seed);

	for (auto Kvp : RoomGenerationData)
	{
		// Fill required rooms
		for (int i = 0; i < Kvp.Value.RequiredInstances; i++)
		{
			RequiredRooms.Add(Kvp.Key);
		}

		// Fill available rooms
		for (int i = 0; i < Kvp.Value.MaximumInstances; i++)
		{
			AvailableRooms.Add(Kvp.Key);
		}

		// Fill spawn pool
		for (int i = 0; i < Kvp.Value.SpawnPoolAmount; i++)
		{
			SpawnPool.Add(Kvp.Key);
		}

		// Construct pre spawn validators
		for (int i = 0; i < Kvp.Value.PreSpawnValidator.Num(); i++)
		{
			TSubclassOf<ULayoutGenerator_SpawnValidator> ValidatorClass;
			ULayoutGenerator_SpawnValidator* Validator;

			ValidatorClass = Kvp.Value.PreSpawnValidator[i];

			if (!SpawnValidators.Contains(ValidatorClass) && ValidatorClass != nullptr)
			{
				Validator = NewObject<ULayoutGenerator_SpawnValidator>(this, ValidatorClass);
				Validator->RandomStream = FRandomStream(RandomStream.RandHelper(1000000));
				SpawnValidators.Add(ValidatorClass, Validator);
			}
		}

		// Construct post spawn validators
		for (int i = 0; i < Kvp.Value.PostSpawnValidator.Num(); i++)
		{
			TSubclassOf<ULayoutGenerator_SpawnValidator> ValidatorClass;
			ULayoutGenerator_SpawnValidator* Validator;

			ValidatorClass = Kvp.Value.PostSpawnValidator[i];

			if (!SpawnValidators.Contains(ValidatorClass) && ValidatorClass != nullptr)
			{
				Validator = NewObject<ULayoutGenerator_SpawnValidator>(this, ValidatorClass);
				Validator->RandomStream = FRandomStream(RandomStream.RandHelper(100000000));
				SpawnValidators.Add(ValidatorClass, Validator);
			}
		}
	}

	// Fill grid
	Grid.Empty(GridSize.X * GridSize.Y);

	for (int x = 0; x < GridSize.X; x++)
	{
		for (int y = 0; y < GridSize.Y; y++)
		{
			ULayoutGenerator_Cell* Cell;

			Cell = NewObject<ULayoutGenerator_Cell>(this, ULayoutGenerator_Cell::StaticClass());
			Cell->LayoutGenerator = this;
			Cell->Location = FIntVector2D(x, y);
			Cell->UniqueSeed = RandomStream.RandHelper(1000000);
			Grid.Add(FIntVector2D(x, y), Cell);
		}
	}

	// Fill queue (changing this will change layout generation)
	for (auto Kvp : Grid)
	{
		Queue.Add(Kvp.Key);
	}
	////

	return true;
}

bool ALayoutGenerator_Main::GenerateRequiredRooms(ELayoutGeneratorErrors& OutError)
{
	// Generate required rooms //
	while (RequiredRooms.Num() > 0)
	{
		if (CurrentInteration > MaximumIterations)
		{
			OutError = ELayoutGeneratorErrors::LGE_MaximumIterationReached;
			return false;
		}

		CurrentInteration++;

		FIntVector2D CellLocation;
		FName RoomRowName;
		bool bRoomSpawned;

		CellLocation = Queue[RandomStream.RandHelper(Queue.Num() - 1)];
		RoomRowName = RequiredRooms[0];
		bRoomSpawned = GetCell(CellLocation)->SetRoom(RoomRowName, false);

		if (bRoomSpawned)
		{
			Queue.RemoveSingle(CellLocation);
			RequiredRooms.RemoveSingle(RoomRowName);
			AvailableRooms.RemoveSingle(RoomRowName);

			if (AvailableRooms.Find(RoomRowName) == INDEX_NONE)
			{
				SpawnPool.Remove(RoomRowName);
			}
		}
	}
	/////

	return true;
}

bool ALayoutGenerator_Main::GenerateFromQueue(ELayoutGeneratorErrors& OutError)
{
	// Generate all cells that are in queue //
	while (Queue.Num() > 0)
	{
		TArray<FName> LocalSpawnPool;

		LocalSpawnPool = SpawnPool;

		// Loop as long as there are still entires left in the local spawn pool 
		while (LocalSpawnPool.Num() > 0)
		{
			if (CurrentInteration > MaximumIterations)
			{
				OutError = ELayoutGeneratorErrors::LGE_MaximumIterationReached;
				return false;
			}

			CurrentInteration++;

			FIntVector2D CellLocation;
			FName RoomRowName;
			bool bRoomSpawned;

			CellLocation = Queue[0];
			RoomRowName = SpawnPool[RandomStream.RandHelper(SpawnPool.Num() - 1)];

			if (!Grid[CellLocation]->bIsEnabled || Grid[CellLocation]->bIsGenerated)
			{
				bRoomSpawned = true;
			}
			else
			{
				bRoomSpawned = GetCell(CellLocation)->SetRoom(RoomRowName, false);
			}

			if (bRoomSpawned)
			{
				Queue.RemoveSingle(CellLocation);
				AvailableRooms.RemoveSingle(RoomRowName);

				if (AvailableRooms.Find(RoomRowName) == INDEX_NONE)
				{
					SpawnPool.Remove(RoomRowName);
				}
				break;
			}
			else 
			{
				// Remove from local spawn pool so room can't be selected anymore for this cell
				LocalSpawnPool.Remove(RoomRowName);
			}
		}

		// If local spawn pool is empty then we did not found a valid room for this cell, so we abort generation
		if (LocalSpawnPool.Num() <= 0)
		{
			OutError = ELayoutGeneratorErrors::LGE_CellFoundNoValidRoom;
			return false;
		}
	}
	////

	return true;
}

bool ALayoutGenerator_Main::RunPostSpawnValidation(ELayoutGeneratorErrors& OutError)
{
	// Run post spawn validation //
	for (auto Kvp : Grid)
	{
		if (Kvp.Value->bIsGenerated)
		{
			bool bIsValid;
			FRoomGenerationSettings SourceSettings;

			bIsValid = true;
			SourceSettings = RoomGenerationData[Kvp.Value->RoomRowName];

			for (auto Elem : SourceSettings.PostSpawnValidator)
			{
				bIsValid = bIsValid && Elem != nullptr && SpawnValidators[Elem]->IsSpawnValid(this, Kvp.Key);
			}

			if (!bIsValid)
			{
				OutError = ELayoutGeneratorErrors::LGE_PostSpawnValidationFailed;
				return false;
			}
		}
	}
	////

	return true;
}

void ALayoutGenerator_Main::ResetRuntimeProperties()
{
	RandomStream = FRandomStream(0);
	RoomGenerationData.Empty();
	RequiredRooms.Empty();
	AvailableRooms.Empty();
	SpawnPool.Empty();
	Queue.Empty();
	CurrentInteration = 0;
	LevelsCurrentlyLoading = 0;

	// Clear spawn validator
	for (auto Kvp : SpawnValidators)
	{
		Kvp.Value->ConditionalBeginDestroy();
	}

	SpawnValidators.Empty();

	// Clear grid
	for (auto Kvp : Grid)
	{
		Kvp.Value->ConditionalBeginDestroy();
	}

	Grid.Empty();

	return;
}

void ALayoutGenerator_Main::OnLevelLoadedCallback()
{
	float PercentOfLevelsDone;

	LevelsCurrentlyLoading--;
	PercentOfLevelsDone = ((float)Grid.Num() - LevelsCurrentlyLoading) / (float)Grid.Num();
	OnLayoutGenerationStatusUpdate.Broadcast(.7f + (PercentOfLevelsDone * .3f), ELayoutGeneratorTasks::LGT_LoadingLevels);

	if (LevelsCurrentlyLoading < 1)
	{
		bIsCurrentlyGeneratingLayout = false;
		bIsLayoutPresent = true;

		OnLayoutGenerationStatusUpdate.Broadcast(1.f, ELayoutGeneratorTasks::LGT_None);
		OnLayoutGenerated.Broadcast(true, ELayoutGeneratorErrors::LGE_None);
		UE_LOG(LogLayoutGenerator, Display, TEXT("%s: Finished generating layout."), *GetName());
	}

	return;
}
