


#include "LayoutGenerator_RoomSpawner.h"
#include "LayoutGenerator_Main.h"
#include "LayoutGenerator_SpawnValidator.h"
#include "BaseRoom.h"

// Sets default values
ALayoutGenerator_RoomSpawner::ALayoutGenerator_RoomSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BasicSphere(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));

	DefaultSceneRoot = CreateDefaultSubobject<UStaticMeshComponent>("Default Scene Root");
	DefaultSceneRoot->SetupAttachment(RootComponent);
	DefaultSceneRoot->SetStaticMesh(BasicSphere.Object);

	OverlapSphere = CreateDefaultSubobject<USphereComponent>("Overlap Sphere");
	OverlapSphere->SetupAttachment(DefaultSceneRoot);
	OverlapSphere->SetSphereRadius(64.f, false);
}

// Called when the game starts or when spawned
void ALayoutGenerator_RoomSpawner::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALayoutGenerator_RoomSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ALayoutGenerator_RoomSpawner::SpawnRoom(FRoomGenerationSettings RoomGenSettings, FRandomStream SeedStream)
{
	return false;
}

bool ALayoutGenerator_RoomSpawner::HasPassedPostSpawnValidation()
{
	return false;
}

void ALayoutGenerator_RoomSpawner::LoadLevel(FLoadingLevelDone OnLoadingFinished)
{
}

