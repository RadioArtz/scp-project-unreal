

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "LayoutGenerator_Structs.h"
#include "Kismet/GameplayStatics.h"
#include "LayoutGenerator_Main.generated.h"

UDELEGATE()
DECLARE_DYNAMIC_DELEGATE_TwoParams(FLayoutGenerationDelegate, bool, bSuccess, FString, ErrorMessage);

DECLARE_LOG_CATEGORY_EXTERN(LogLayoutGenerator, Log, All);

class ULayoutGenerator_SpawnValidator;

UCLASS()
class SCPPU_API ALayoutGenerator_Main : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALayoutGenerator_Main();

	//// FUNCTIONS ////

	/** Generates a new layout. Use the seed variable to set the seed. Ignores calls if a layout is already present. */
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "OnDone"))
		void AsyncGenerateLayout(const int32 NewSeed, const FLayoutGenerationDelegate& OnDone);

	/** Clears the layout. Ignores calls if no layout is present. */
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "OnDone"))
		void AsyncClearLayout(const FLayoutGenerationDelegate& OnDone);

	/**
	*Tries to set a room at a given cell location.
	*@param CellLocation		Cell location where to set the room.
	*@param RowName				Row name of the room inside the datatable.
	*@param bForce				Wether to force set the room or not.
	*@param bUpdateNeighbours	Update neigbours accordingly. Will only have an effect when forcing. THIS IS HIGHLY RECOMMENDED. NOT YET IMPLEMENTED.
	*@return					Returns if a room was spawned or not.
	*/
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Set Room (Do not use in current state!)"))
		bool SetRoom(const FIntVector2D CellLocation, const FName RoomRowName, const bool bForce, const bool bUpdateNeighbours = true);

	UFUNCTION(BlueprintCallable)
		TArray<FIntVector2D> FindCellLocationsWithRoomRowName(const FName RoomRowName);

	UFUNCTION(BlueprintCallable)
		FGridCell GetCellAtCellLocation(const FIntVector2D CellLocation);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Does Path Exists (Currently always returns false!)"))
		bool DoesPathExist(const FIntVector2D Start, const FIntVector2D End);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FVector CellLocationToWorldLocation(const FIntVector2D CellLocation);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FRotator CellRotationToWorldRotation(const int32 CellRotation);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FString GetUniqueLevelName(const FIntVector2D CellLocation);

	//// PROPERTIES ////

	/** Size of the grid. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		FIntVector2D GridSize =  FIntVector2D(5, 5);

	/** Size of an individuel cell (in uu). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings", meta = (ClampMin = "1"))
		float CellSize = 1024;

	/** Datatable that specifies which rooms to generate. Has to be of type FRoomGenerationSettings. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		UDataTable* DataTable;

	/** Used to prevent a infinte loop */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (ClampMin = "1"))
		int32 MaximumIterations = 1000000;

	/** Seed used for layout generation. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime")
		int32 Seed;

	/** Returns if a layout is already present. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime")
		bool bIsLayoutPresent;

	/** Returns if a layout is currently generated. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Runtime")
		bool bIsCurrentlyGeneratingLayout;

protected:
	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	/** Called every frame */
	virtual void Tick(float DeltaTime) override;

	//// FUNCTIONS ////

	UFUNCTION()
		void InitRuntimeProperties();

	UFUNCTION()
		void OnAssetsLoaded();

	UFUNCTION()
		void GenLayout();

	UFUNCTION()
		void LoadRoomLevels();

	UFUNCTION()
		void OnLevelInstanceLoaded();

	/** Clears all runtime properties used to generate the layout. */
	UFUNCTION()
		void ClearRuntimeProperties();

	/** Draws a preview of the grid. */
	UFUNCTION()
		void DrawGridPreview();

	//// PROPERTIES ////

	UPROPERTY(VisibleAnywhere, Category = "DEBUG")
		FLayoutGenerationDelegate OnTaskDone;

	/** Random stream created from seed variable. */
	UPROPERTY(VisibleAnywhere, Category = "DEBUG")
		FRandomStream RStream;

	/** Cached datatable to ensure thread saftey and for ease of use. */
	UPROPERTY(VisibleAnywhere, Category = "DEBUG")
		TMap<FName, FRoomGenerationSettings> RoomGenerationData;

	/** Contains row names and how many instances of it are still required. Coresponse to RequiredInstances inside FRoomGenerationSettings. */
	UPROPERTY(VisibleAnywhere, Category = "DEBUG")
		TArray<FName> RequiredRooms;

	/** Contains row names and how many instances of it are still available. Coresponse to MaximumInstances inside FRoomGenerationSettings. */
	UPROPERTY(VisibleAnywhere, Category = "DEBUG")
		TArray<FName> AvailableRooms;

	/** Contains row names. Coresponse to SpawnPoolAmount inside FRoomGenerationSettings. */
	UPROPERTY(VisibleAnywhere, Category = "DEBUG")
		TArray<FName> SpawnPool;

	/** Cell generation order. */
	UPROPERTY(VisibleAnywhere, Category = "DEBUG")
		TArray<FIntVector2D> Queue;

	/** Current iteration */
	UPROPERTY(VisibleAnywhere, Category = "DEBUG")
		int32 CurrentInteration;

	/** Cached spawn validator objects*/
	UPROPERTY(VisibleAnywhere, Category = "DEBUG")
		TMap<TSubclassOf<ULayoutGenerator_SpawnValidator>, ULayoutGenerator_SpawnValidator*> SpawnValidators;

	/** Number of Maps Instances currently loading*/
	UPROPERTY(VisibleAnywhere, Category = "DEBUG")
		int32 CurrentLoadingLevelInstances;

	/** Contains the actual grid. */
	UPROPERTY(VisibleAnywhere, Category = "DEBUG")
		TMap<FIntVector2D, FGridCell> Grid;

};
