

#include "LayoutGenerator_Main.h"
#include "Components/StaticMeshComponent.h" 
#include "Engine/LevelStreamingDynamic.h" 
#include "Engine/AssetManager.h" 
#include "Engine/StreamableManager.h" 
#include "DrawDebugHelpers.h" 
#include "LayoutGenerator_SpawnValidator.h"

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

void ALayoutGenerator_Main::AsyncGenerateLayout(const int32 NewSeed, const FLayoutGenerationDelegate& OnDone)
{
	UE_LOG(LogLayoutGenerator, Display, TEXT("%s: Starting layout generation..."), *GetName());

	if (bIsCurrentlyGeneratingLayout || bIsLayoutPresent)
	{
		OnDone.ExecuteIfBound(false, "Layout already present");
		UE_LOG(LogLayoutGenerator, Error, TEXT("%s: Failed to generate layout: Layout already present"), *GetName());
		return;
	}

	bIsCurrentlyGeneratingLayout = true;
	OnTaskDone = OnDone;
	this->Seed = NewSeed;

	// Init runtime properties on game thread//
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		ClearRuntimeProperties();
		InitRuntimeProperties();
	});
	////

	/** InitRuntimeProperties continues generation */
}

void ALayoutGenerator_Main::AsyncClearLayout(const FLayoutGenerationDelegate& OnDone)
{
}

bool ALayoutGenerator_Main::SetRoom(const FIntVector2D CellLocation, const FName RoomRowName, const bool bForce, const bool bUpdateNeighbours)
{
	FGridCell* ThisCell;
	FGridCell* PosXCell;
	FGridCell* PosYCell;
	FGridCell* NegXCell;
	FGridCell* NegYCell;
	FGridCell ThisCellPrevious;
	FRoomGenerationSettings SourceSettings;
	FCellSides IsDoorRequired;
	FCellSides IsDoorBlocked;
	bool bSuccess;

	ThisCell = &Grid[CellLocation];
	ThisCellPrevious = *ThisCell;
	ThisCell->RoomRowName = RoomRowName;
	SourceSettings = RoomGenerationData[ThisCell->RoomRowName];
	ThisCell->HasDoor = SourceSettings.HasDoor;
	ThisCell->ShouldDisableNeighbour = SourceSettings.ShouldDisableNeighbour;

	// Abort cell generation if (cell is disabled OR already generated) AND we don't force it
	if ((!ThisCell->bIsEnabled || ThisCell->bIsGenerated) && !bForce)
	{
		return false;
	}

	// Fetch info from neighbouring cells //
	PosXCell = Grid.Find(FIntVector2D(CellLocation.X + 1, CellLocation.Y));
	PosYCell = Grid.Find(FIntVector2D(CellLocation.X, CellLocation.Y + 1));
	NegXCell = Grid.Find(FIntVector2D(CellLocation.X - 1, CellLocation.Y));
	NegYCell = Grid.Find(FIntVector2D(CellLocation.X, CellLocation.Y - 1));

	// Positive X
	IsDoorRequired.bPositiveX = (PosXCell != nullptr && PosXCell->HasDoor.bNegativeX);
	IsDoorBlocked.bPositiveX = (PosXCell == nullptr || !PosXCell->bIsEnabled) || (PosXCell->bIsGenerated && !PosXCell->HasDoor.bNegativeX);

	// Positive Y
	IsDoorRequired.bPositiveY = (PosYCell != nullptr && PosYCell->HasDoor.bNegativeY);
	IsDoorBlocked.bPositiveY = (PosYCell == nullptr || !PosYCell->bIsEnabled) || (PosYCell->bIsGenerated && !PosYCell->HasDoor.bNegativeY);

	// Negative X
	IsDoorRequired.bNegativeX = (NegXCell != nullptr && NegXCell->HasDoor.bPositiveX);
	IsDoorBlocked.bNegativeX = (NegXCell == nullptr || !NegXCell->bIsEnabled) || (NegXCell->bIsGenerated && !NegXCell->HasDoor.bPositiveX);

	// Negative Y
	IsDoorRequired.bNegativeY = (NegYCell != nullptr && NegYCell->HasDoor.bPositiveY);
	IsDoorBlocked.bNegativeY = (NegYCell == nullptr || !NegYCell->bIsEnabled) || (NegYCell->bIsGenerated && !NegYCell->HasDoor.bPositiveY);

	// Check if the room fits and when not rotate it
	for (int i = 0; i < 4; i++)
	{
		bSuccess = true;

		// Positive X
		bSuccess = bSuccess && ((IsDoorRequired.bPositiveX && ThisCell->HasDoor.bPositiveX) || !IsDoorRequired.bPositiveX);
		bSuccess = bSuccess && ((IsDoorBlocked.bPositiveX && !ThisCell->HasDoor.bPositiveX) || !IsDoorBlocked.bPositiveX);

		// Positive Y
		bSuccess = bSuccess && ((IsDoorRequired.bPositiveY && ThisCell->HasDoor.bPositiveY) || !IsDoorRequired.bPositiveY);
		bSuccess = bSuccess && ((IsDoorBlocked.bPositiveY && !ThisCell->HasDoor.bPositiveY) || !IsDoorBlocked.bPositiveY);

		// Negative X
		bSuccess = bSuccess && ((IsDoorRequired.bNegativeX && ThisCell->HasDoor.bNegativeX) || !IsDoorRequired.bNegativeX);
		bSuccess = bSuccess && ((IsDoorBlocked.bNegativeX && !ThisCell->HasDoor.bNegativeX) || !IsDoorBlocked.bNegativeX);

		// Negative Y
		bSuccess = bSuccess && ((IsDoorRequired.bNegativeY && ThisCell->HasDoor.bNegativeY) || !IsDoorRequired.bNegativeY);
		bSuccess = bSuccess && ((IsDoorBlocked.bNegativeY && !ThisCell->HasDoor.bNegativeY) || !IsDoorBlocked.bNegativeY);

		// Check pre spawn validators
		for (auto Elem : SourceSettings.PreSpawnValidator)
		{
			bSuccess = bSuccess && Elem != nullptr && SpawnValidators[Elem]->IsSpawnValid(this, CellLocation);
		}

		if (bSuccess)
		{
			break;
		}
		else
		{
			// Rotate the room right
			FCellSides PreviousDoorLocation = ThisCell->HasDoor;

			ThisCell->HasDoor.bPositiveX = PreviousDoorLocation.bNegativeY;
			ThisCell->HasDoor.bPositiveY = PreviousDoorLocation.bPositiveX;
			ThisCell->HasDoor.bNegativeX = PreviousDoorLocation.bPositiveY;
			ThisCell->HasDoor.bNegativeY = PreviousDoorLocation.bNegativeX;
			
			ThisCell->Rotation += 1;
		}
	}

	if (bSuccess)
	{
		ThisCell->bIsGenerated = true;

		// Disable neighbouring cells if needed
		if (PosXCell != nullptr)
		{
			PosXCell->bIsEnabled = PosXCell->bIsEnabled && !ThisCell->ShouldDisableNeighbour.bPositiveX;
		}

		if (PosYCell != nullptr)
		{
			PosYCell->bIsEnabled = PosYCell->bIsEnabled && !ThisCell->ShouldDisableNeighbour.bPositiveY;
		}

		if (NegXCell != nullptr)
		{
			NegXCell->bIsEnabled = NegXCell->bIsEnabled && !ThisCell->ShouldDisableNeighbour.bNegativeX;
		}

		if (NegYCell != nullptr)
		{
			NegYCell->bIsEnabled = NegYCell->bIsEnabled && !ThisCell->ShouldDisableNeighbour.bNegativeY;
		}

		return true;
	}
	else
	{
		*ThisCell = ThisCellPrevious;
		return false;
	}
}

TArray<FIntVector2D> ALayoutGenerator_Main::FindCellLocationsWithRoomRowName(const FName RoomRowName)
{
	TArray<FIntVector2D> FoundCellLocations;

	for (auto Kvp : Grid)
	{
		if (Kvp.Value.RoomRowName == RoomRowName)
		{
			FoundCellLocations.AddUnique(Kvp.Key);
		}
	}

	return FoundCellLocations;
}

FGridCell ALayoutGenerator_Main::GetCellAtCellLocation(const FIntVector2D CellLocation)
{
	return *Grid.Find(CellLocation);
}

bool ALayoutGenerator_Main::DoesPathExist(const FIntVector2D Start, const FIntVector2D End)
{
	return false;
}

FVector ALayoutGenerator_Main::CellLocationToWorldLocation(const FIntVector2D CellLocation)
{
	return FVector(CellLocation.X * CellSize + GetActorLocation().X, CellLocation.Y * CellSize + GetActorLocation().Y, GetActorLocation().Z);
}

FRotator ALayoutGenerator_Main::CellRotationToWorldRotation(const int32 CellRotation)
{
	return FRotator(0, CellRotation * 90, 0);
}

FString ALayoutGenerator_Main::GetUniqueLevelName(const FIntVector2D CellLocation)
{
	FString UniqueLevelName;

	UniqueLevelName = GetName();
	UniqueLevelName.Append("__X");
	UniqueLevelName.AppendInt(CellLocation.X);
	UniqueLevelName.Append("_Y");
	UniqueLevelName.AppendInt(CellLocation.Y);
	return UniqueLevelName;
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

void ALayoutGenerator_Main::InitRuntimeProperties()
{
	// DataTable initialisation //
	if (DataTable == nullptr)
	{
		bIsCurrentlyGeneratingLayout = false;
		OnTaskDone.ExecuteIfBound(false, "No datatable present");
		UE_LOG(LogLayoutGenerator, Error, TEXT("%s: Failed to generate layout: No datatable present"), *GetName());
		return;
	}

	if (DataTable->RowStruct != FRoomGenerationSettings::StaticStruct())
	{
		bIsCurrentlyGeneratingLayout = false;
		OnTaskDone.ExecuteIfBound(false, "Datatable has wrong sturct type");
		UE_LOG(LogLayoutGenerator, Error, TEXT("%s: Failed to generate layout: Datatable has wrong sturct type"), *GetName());
		return;
	}

	for (auto Kvp : DataTable->GetRowMap())
	{
		RoomGenerationData.Add(Kvp.Key, *(FRoomGenerationSettings*)Kvp.Value);
	}
	////

	// Properties initialisation //
	RStream = FRandomStream(Seed);

	for (auto Kvp : RoomGenerationData)
	{
		// Required rooms
		for (int i = 0; i < Kvp.Value.RequiredInstances; i++)
		{
			RequiredRooms.Add(Kvp.Key);
		}

		// Available rooms
		for (int i = 0; i < Kvp.Value.MaximumInstances; i++)
		{
			AvailableRooms.Add(Kvp.Key);
		}

		// Spawn pool
		for (int i = 0; i < Kvp.Value.SpawnPoolAmount; i++)
		{
			SpawnPool.Add(Kvp.Key);
		}

		// Pre spawn validators
		for (int i = 0; i < Kvp.Value.PreSpawnValidator.Num(); i++)
		{
			TSubclassOf<ULayoutGenerator_SpawnValidator> ValidatorClass;
			ULayoutGenerator_SpawnValidator* Validator;

			ValidatorClass = Kvp.Value.PreSpawnValidator[i];

			if (!SpawnValidators.Contains(ValidatorClass) && ValidatorClass != nullptr)
			{
				Validator = NewObject<ULayoutGenerator_SpawnValidator>(this, ValidatorClass);
				SpawnValidators.Add(ValidatorClass, Validator);
			}
		}

		// Post spawn validators
		for (int i = 0; i < Kvp.Value.PostSpawnValidator.Num(); i++)
		{
			TSubclassOf<ULayoutGenerator_SpawnValidator> ValidatorClass;
			ULayoutGenerator_SpawnValidator* Validator;

			ValidatorClass = Kvp.Value.PostSpawnValidator[i];

			if (!SpawnValidators.Contains(ValidatorClass) && ValidatorClass != nullptr)
			{
				Validator = NewObject<ULayoutGenerator_SpawnValidator>(this, ValidatorClass);
				SpawnValidators.Add(ValidatorClass, Validator);
			}
		}
	}

	// Grid
	Grid.Empty(GridSize.X * GridSize.Y);

	for (int x = 0; x < GridSize.X; x++)
	{
		for (int y = 0; y < GridSize.Y; y++)
		{
			Grid.Add(FIntVector2D(x, y), FGridCell());
		}
	}

	// Queue (changing this will change layout generation)
	for (auto Kvp : Grid)
	{
		Queue.Add(Kvp.Key);
	}
	////

	// Load Assets //
	FStreamableManager* StreamManager;
	FStreamableDelegate OnDoneCallback;
	TArray<FSoftObjectPath> LevelAssets;
	FString DebugName;

	StreamManager = &GEngine->AssetManager->GetStreamableManager();
	DebugName = GetName();
	DebugName.Append("_LevelAssets");
	OnDoneCallback.BindUFunction(this, "OnAssetsLoaded");

	for (auto Kvp : RoomGenerationData)
	{
		for (int i = 0; i < Kvp.Value.Maps.Num(); i++)
		{
			LevelAssets.AddUnique(Kvp.Value.Maps[i].ToSoftObjectPath());
		}
	}

	StreamManager->RequestAsyncLoad(LevelAssets, OnDoneCallback, 1, false, false, DebugName);
	UE_LOG(LogLayoutGenerator, Display, TEXT("%s: Loading assets..."), *GetName());
	////

	/** OnAssetsLoaded continues generation */
}

void ALayoutGenerator_Main::OnAssetsLoaded()
{
	UE_LOG(LogLayoutGenerator, Display, TEXT("%s: Loading assets done"), *GetName());

	// Generate layout on another thread //
	AsyncTask(ENamedThreads::AnyBackgroundHiPriTask, [this]()
	{
		GenLayout();
	});
	////

	/** GenLayout continues generation */
}

void ALayoutGenerator_Main::GenLayout()
{
	UE_LOG(LogLayoutGenerator, Display, TEXT("%s: Generating rooms..."), *GetName());

	// Generate required rooms //
	while (RequiredRooms.Num() > 0)
	{
		if (CurrentInteration > MaximumIterations)
		{
			bIsCurrentlyGeneratingLayout = false;
			ClearRuntimeProperties();
			OnTaskDone.ExecuteIfBound(false, "Maximum iteration reached");
			UE_LOG(LogLayoutGenerator, Error, TEXT("%s: Failed to generate layout: Maximum iteration reached"), *GetName());
			return;
		}

		CurrentInteration++;

		FIntVector2D CellLocation;
		FName RoomRowName;
		bool bRoomSpawned;

		CellLocation = Queue[RStream.RandHelper(Queue.Num() - 1)];
		RoomRowName = RequiredRooms[0];
		bRoomSpawned = SetRoom(CellLocation, RoomRowName, false, false);

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
				bIsCurrentlyGeneratingLayout = false;
				ClearRuntimeProperties();
				OnTaskDone.ExecuteIfBound(false, "Maximum iteration reached");
				UE_LOG(LogLayoutGenerator, Error, TEXT("%s: Failed to generate layout: Maximum iteration reached"), *GetName());
				return;
			}

			CurrentInteration++;

			FIntVector2D CellLocation;
			FName RoomRowName;
			bool bRoomSpawned;

			CellLocation = Queue[0];
			RoomRowName = SpawnPool[RStream.RandHelper(SpawnPool.Num() - 1)];

			if (!Grid[CellLocation].bIsEnabled || Grid[CellLocation].bIsGenerated)
			{
				bRoomSpawned = true;
			}
			else
			{
				bRoomSpawned = SetRoom(CellLocation, RoomRowName, false, false);
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
			bIsCurrentlyGeneratingLayout = false;
			ClearRuntimeProperties();
			OnTaskDone.ExecuteIfBound(false, "Cell found no valid room to generate");
			UE_LOG(LogLayoutGenerator, Error, TEXT("%s: Failed to generate layout: Cell found no valid room to generate"), *GetName());
			return;
		}
	}
	////

	UE_LOG(LogLayoutGenerator, Display, TEXT("%s: Generating rooms done"), *GetName());

	// Run post spawn validation //
	UE_LOG(LogLayoutGenerator, Display, TEXT("%s: Running post spawn validation..."), *GetName());

	for (auto Kvp : Grid)
	{
		bool bIsValid;
		FRoomGenerationSettings SourceSettings;

		bIsValid = true;
		SourceSettings = RoomGenerationData[Kvp.Value.RoomRowName];

		for (auto Elem : SourceSettings.PostSpawnValidator)
		{
			bIsValid = bIsValid && Elem != nullptr && SpawnValidators[Elem]->IsSpawnValid(this, Kvp.Key);
		}

		if (!bIsValid)
		{
			bIsCurrentlyGeneratingLayout = false;
			ClearRuntimeProperties();
			OnTaskDone.ExecuteIfBound(false, "Post spawn validation failed");
			UE_LOG(LogLayoutGenerator, Error, TEXT("%s: Failed to generate layout: Post spawn validation failed"), *GetName());
			return;
		}
	}

	UE_LOG(LogLayoutGenerator, Display, TEXT("%s: Running post spawn validation done"), *GetName());
	////

	// Load level instances on game thread //
	AsyncTask(ENamedThreads::GameThread, [this]()
	{
		LoadRoomLevels();
	});
	////

	/** LoadRoomLevels continues generation */
}

void ALayoutGenerator_Main::LoadRoomLevels()
{
	// Load room level instances //
	UE_LOG(LogLayoutGenerator, Display, TEXT("%s: Spawning level instances..."), *GetName());

	for (auto Kvp : Grid)
	{
		FVector LevelWorldLocation;
		FRotator LevelWorldRotation;
		FString LevelOverrideName;
		ULevelStreamingDynamic* LStreamingDyn;
		TArray<TSoftObjectPtr<UWorld> > LevelAssets;
		bool bSuccess;

		LevelWorldLocation = CellLocationToWorldLocation(Kvp.Key);
		LevelWorldRotation = CellRotationToWorldRotation(Kvp.Value.Rotation);
		LevelOverrideName = GetUniqueLevelName(Kvp.Key);
		LevelAssets = RoomGenerationData[Kvp.Value.RoomRowName].Maps;

		if (LevelAssets.Num() > 0 && Kvp.Value.bIsEnabled && Kvp.Value.bIsGenerated)
		{
			LStreamingDyn = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(GetWorld(), LevelAssets[RStream.RandHelper(LevelAssets.Num() - 1)], LevelWorldLocation, LevelWorldRotation, bSuccess, LevelOverrideName);

			if (bSuccess)
			{
				LStreamingDyn->OnLevelShown.AddDynamic(this, &ALayoutGenerator_Main::OnLevelInstanceLoaded);
				CurrentLoadingLevelInstances++;
			}
		}
	}
	////

	/** OnLevelInstanceLoaded continues generation */
}

void ALayoutGenerator_Main::OnLevelInstanceLoaded()
{
	CurrentLoadingLevelInstances--;

	if (CurrentLoadingLevelInstances <= 0)
	{
		UE_LOG(LogLayoutGenerator, Display, TEXT("%s: Spawning level instances done"), *GetName());

		bIsLayoutPresent = true;
		bIsCurrentlyGeneratingLayout = false;
		ClearRuntimeProperties();
		OnTaskDone.ExecuteIfBound(true, "");
		UE_LOG(LogLayoutGenerator, Display, TEXT("%s: Finished layout generation"), *GetName());
		/** Generation finished */
		return;
	}

}

void ALayoutGenerator_Main::ClearRuntimeProperties()
{
	RStream = FRandomStream(0);
	RoomGenerationData.Empty();
	RequiredRooms.Empty();
	AvailableRooms.Empty();
	SpawnPool.Empty();
	Queue.Empty();
	SpawnValidators.Empty();
	CurrentInteration = 0;
	CurrentLoadingLevelInstances = 0;
}

void ALayoutGenerator_Main::DrawGridPreview()
{
}
