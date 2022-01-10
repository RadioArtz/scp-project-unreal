


#include "LayoutGenerator_Main.h"
#include "LayoutGenerator_RoomSpawner.h"
#include "BaseRoom.h"
#include "DrawDebugHelpers.h" 

// Sets default values
ALayoutGenerator_Main::ALayoutGenerator_Main()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BasicSphere(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));

	DefaultSceneRoot = CreateDefaultSubobject<UStaticMeshComponent>("Default Scene Root");
	DefaultSceneRoot->SetupAttachment(RootComponent);
	DefaultSceneRoot->SetStaticMesh(BasicSphere.Object);
}

// Called in editor on values change
void ALayoutGenerator_Main::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	ClampVariables();
	GenerateGrid();
}

// Called when the game starts or when spawned
void ALayoutGenerator_Main::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called when the actor gets destroyed
void ALayoutGenerator_Main::BeginDestroy()
{
	Super::BeginDestroy();
	FlushPersistentDebugLines(GetWorld());
}

// Called every frame
void ALayoutGenerator_Main::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALayoutGenerator_Main::GenerateLayout(FLayoutGenerationDone OnDone)
{
	if (bHasLayoutGenerated)
	{
		OnDone.ExecuteIfBound(false, "layout already generated");
		return;
	}

	// Cache datatable so we don't need to cast our struct every time
	CachedDataTable.Empty();
	if (DataTable == nullptr)
	{
		OnDone.ExecuteIfBound(false, "no datatable found");
		return;
	}

	for (auto Row : DataTable->GetRowMap())
	{
		CachedDataTable[Row.Key] = *(FRoomGenerationSettings*)(Row.Value);
	}

	if (CachedDataTable.Num() == 0)
	{
		OnDone.ExecuteIfBound(false, "datatable returned nothing");
		return;
	}

	// Prepare variables based on the datatable
	for (auto Row : CachedDataTable)
	{
		RoomInstances[Row.Key] = 0;

		RequiredRooms.Empty();
		for (int32 i = 0; i < Row.Value.MinimumInstances; i++)
		{
			RequiredRooms.Add(Row.Key);
		}

		SpawnPool.Empty();
		for (int32 i = 0; i < Row.Value.SpawnPoolAmount; i++)
		{
			SpawnPool.Add(Row.Key);
		}
	}

	ClampVariables();
	GenerateGrid();

	// Spawn room spawner for every grid entry
	for (auto GridElement : Grid)
	{
		ALayoutGenerator_RoomSpawner* RoomSpawner = GetWorld()->SpawnActor<ALayoutGenerator_RoomSpawner>(GridLocationToWorldLocation(GridElement.Key), FRotator());
		RoomSpawner->LayoutGenerator = this;
		RoomSpawner->GridLocation = GridElement.Key;
		GridElement.Value.RoomSpawner = RoomSpawner;
	}

	// Generate required rooms first
	for (auto RowName : RequiredRooms)
	{
		// Try 100 random grid entries and break if we find an entry where the room can spawn
		for (int32 i = 0; i < 100; i++)
		{
			int32 RandomX = SeedStream.RandRange(0, GridSize.X - 1);
			int32 RandomY = SeedStream.RandRange(0, GridSize.Y - 1);
			FGridEntry RandomGridEntry = Grid[FIntVector2D(RandomX, RandomY)];

			FRoomGenerationSettings RoomGenSettings = CachedDataTable[RowName];
			FRandomStream RoomSeedStream = FRandomStream(SeedStream.GetUnsignedInt());
			
			bool bSuccess = RandomGridEntry.RoomSpawner->SpawnRoom(RoomGenSettings, RoomSeedStream);

			if (bSuccess)
			{
				RoomInstances[RowName] += 1;

				// Remove room from spawn pool if it reached its max instance value
				if (RoomInstances[RowName] >= RoomGenSettings.MaximumInstances)
				{
					SpawnPool.RemoveSwap(RowName, true);
				}

				break;
			}
			else if (i == 99)
			{
				// Required room could not be spawned
				OnDone.ExecuteIfBound(false, "required room not generated");
				return;
			}
		}
	}

	// Fill the queue with random entries until we have 10% of the grid added to the queue
	while (Queue.Num() < (GridSize.X * GridSize.Y * 0.1))
	{
		int32 RandomX = SeedStream.RandRange(0, GridSize.X - 1);
		int32 RandomY = SeedStream.RandRange(0, GridSize.Y - 1);

		Queue.Add(FIntVector2D(RandomX, RandomY));
	}

	// Work off the queue and spawn a room at every entry
	// Note that every spawned room will most likey add other entries to the queue
	TArray<FName> LocalSpawnPool = SpawnPool;
	for (int32 i = 0; Queue.Num() > 0; i++)
	{
		FGridEntry QueuedGridEntry = Grid[Queue[0]];
		FName RandomRowName = LocalSpawnPool[SeedStream.RandRange(0, SpawnPool.Num() - 1)];

		FRoomGenerationSettings RoomGenSettings = CachedDataTable[RandomRowName];
		FRandomStream RoomSeedStream = FRandomStream(SeedStream.GetUnsignedInt());

		bool bSuccess = QueuedGridEntry.RoomSpawner->SpawnRoom(RoomGenSettings, RoomSeedStream);

		if (bSuccess)
		{
			RoomInstances[RandomRowName] += 1;

			// Remove room from spawn pool if it reached its max instance value
			if (RoomInstances[RandomRowName] >= RoomGenSettings.MaximumInstances)
			{
				SpawnPool.RemoveSwap(RandomRowName, true);
			}

			LocalSpawnPool = SpawnPool;
			Queue.RemoveAt(0);
		}
		else if (i >= MaximumIterations) // Max iterations so we don't get stuck in an infine loop
		{
			OnDone.ExecuteIfBound(false, "maximum iterations reached");
			return;
		}
		else
		{
			// Remove room from local spawn pool so it doesn't get selected in the next run
			LocalSpawnPool.RemoveSwap(RandomRowName, true);
		}
	}

	// Run post spawn validation
	for (auto GridElement : Grid)
	{
		bool bSuccess = GridElement.Value.RoomSpawner->HasPassedPostSpawnValidation();

		if (!bSuccess)
		{
			OnDone.ExecuteIfBound(false, "post spawn validation failed");
			return;
		}
	}

	// Load level instances
	int32 LoadingLevels = 0;

	auto OnLoadingLevelDone = [](int32 &LoadingLevels, FLayoutGenerationDone &OnDone)
	{
		LoadingLevels -= 1;
		if (&LoadingLevels == 0)
		{
			OnDone.ExecuteIfBound(true, "");
		}
	};

	for (auto GridElement : Grid)
	{
		LoadingLevels += 1;

		FLoadingLevelDone CallbackDelegate = FLoadingLevelDone();
		CallbackDelegate.BindLambda(this, &OnLoadingLevelDone, &LoadingLevels, &OnDone);

		GridElement.Value.RoomSpawner->LoadLevel(CallbackDelegate);
	}

	bHasLayoutGenerated = true;
	return;
}

bool ALayoutGenerator_Main::ClearLayout()
{
	bHasLayoutGenerated = false;
	return true;
}

void ALayoutGenerator_Main::ClampVariables()
{
	GridSize.X = FMath::Clamp(GridSize.X, 1, 100);
	GridSize.Y = FMath::Clamp(GridSize.Y, 1, 100);
	RoomSize = FMath::Clamp(RoomSize, 1.f, 100000.f);
	MaximumIterations = FMath::Clamp(MaximumIterations, 1, 2147483647);
}

void ALayoutGenerator_Main::GenerateGrid()
{
	Grid.Empty(GridSize.X * GridSize.Y);
	FlushPersistentDebugLines(GetWorld());
	
	for (int32 x = 0; x < GridSize.X; x++)
	{
		for (int32 y = 0; y < GridSize.Y; y++)
		{
			Grid.Add(FIntVector2D(x, y), FGridEntry());
			DrawDebugBox(GetWorld(), GridLocationToWorldLocation(FIntVector2D(x, y)), FVector(RoomSize / 2, RoomSize / 2, 0), FColor(255, 0, 0), true);
		}
	}
}

FVector ALayoutGenerator_Main::GridLocationToWorldLocation(FIntVector2D GridLocation)
{
	return FVector(GridLocation.X * RoomSize + (GetActorLocation().X + RoomSize), GridLocation.Y * RoomSize + (GetActorLocation().Y + RoomSize), GetActorLocation().Z);
}

bool ALayoutGenerator_Main::GetGridEntry(FGridEntry &OutGridEntry, FIntVector2D GridLocation)
{
	if (!Grid.Contains(GridLocation))
	{
		return false;
	}

	OutGridEntry = Grid[GridLocation];
	return true;
}

