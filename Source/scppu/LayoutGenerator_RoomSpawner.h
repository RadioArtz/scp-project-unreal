

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h" 
#include "LayoutGenerator_Structs.h"
#include "LayoutGenerator_RoomSpawner.generated.h"

DECLARE_DELEGATE(FLoadingLevelDone);

class ALayoutGenerator_Main;

UCLASS()
class SCPPU_API ALayoutGenerator_RoomSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALayoutGenerator_RoomSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
		UStaticMeshComponent* DefaultSceneRoot;

	UPROPERTY(EditDefaultsOnly)
		USphereComponent* OverlapSphere;

	UPROPERTY(VisibleAnywhere, Category = "DEBUG")
		ALayoutGenerator_Main* LayoutGenerator;

	UPROPERTY(VisibleAnywhere, Category = "DEBUG")
		FIntVector2D GridLocation;

	UPROPERTY(VisibleAnywhere, Category = "DEBUG")
		FRoomGenerationSettings RoomGenerationSettings;

	UPROPERTY(VisibleAnywhere, Category = "DEBUG")
		FRoomSides DoorRequired;

	UPROPERTY(VisibleAnywhere, Category = "DEBUG")
		FRoomSides DoorBlocked;

	UFUNCTION()
	bool SpawnRoom(FRoomGenerationSettings RoomGenSettings, FRandomStream SeedStream);

	UFUNCTION()
	bool HasPassedPostSpawnValidation();

	UFUNCTION()
	void LoadLevel(FLoadingLevelDone OnDone);
};
