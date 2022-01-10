

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "LayoutGenerator_Structs.h"
#include "LayoutGenerator_Main.generated.h"

DECLARE_DYNAMIC_DELEGATE_TwoParams(FLayoutGenerationDone, bool, bSuccess, FString, ErrorReason);

UCLASS()
class SCPPU_API ALayoutGenerator_Main : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALayoutGenerator_Main();

protected:
	// Called in editor on value change
	virtual void OnConstruction(const FTransform& Transform) override;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the actor gets destroyed
	virtual void BeginDestroy() override;
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
		UStaticMeshComponent* DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Generation")
		bool bHasLayoutGenerated = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
		FIntVector2D GridSize = FIntVector2D(5, 5);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
		float RoomSize = 1024;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
		UDataTable* DataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
		FRandomStream SeedStream;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generation")
		int32 MaximumIterations = 1000000;

	UPROPERTY(VisibleAnywhere, Category = "DEBUG")
		TMap<FIntVector2D, FGridEntry> Grid;

	UPROPERTY(VisibleAnywhere, Category = "DEBUG")
		TMap<FName, FRoomGenerationSettings> CachedDataTable;

	UPROPERTY(VisibleAnywhere, Category = "DEBUG")
		TMap<FName, int32> RoomInstances;

	UPROPERTY(VisibleAnywhere, Category = "DEBUG")
		TArray<FName> RequiredRooms;

	UPROPERTY(VisibleAnywhere, Category = "DEBUG")
		TArray<FName> SpawnPool;

	UPROPERTY(VisibleAnywhere, Category = "DEBUG")
		TArray<FIntVector2D> Queue;


	UFUNCTION(BlueprintCallable)
		void GenerateLayout(FLayoutGenerationDone OnDone);

	UFUNCTION(BlueprintCallable)
		bool ClearLayout();

	UFUNCTION()
		void ClampVariables();

	UFUNCTION()
		void GenerateGrid();

	// HELPER FUNCTIONS
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FVector GridLocationToWorldLocation(FIntVector2D GridLocation);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool GetGridEntry(FGridEntry &OutGridEntry, FIntVector2D GridLocation);
};
