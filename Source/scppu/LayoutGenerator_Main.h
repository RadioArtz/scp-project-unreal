

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "LayoutGenerator_Structs.h"
#include "LayoutGenerator_Main.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLayoutGenerator, Log, All);

class ULayoutGenerator_SpawnValidator;
class ULayoutGenerator_Cell;

UENUM(BlueprintType)
enum class ELayoutGeneratorErrors : uint8
{
	LGE_None						= 0		UMETA(DisplayName = "None"),
	LGE_LayoutAlreadyPresent		= 1		UMETA(DisplayName = "Layout already present"),
	LGE_NoDatatable					= 2		UMETA(DisplayName = "No Datatable"),
	LGE_WrongDatatableStruct		= 3		UMETA(DisplayName = "Wrong Datatable Struct"),
	LGE_MaximumIterationReached		= 4		UMETA(DisplayName = "Maximum iteration reached"),
	LGE_CellFoundNoValidRoom		= 5		UMETA(DisplayName = "Cell found no valid room"),
	LGE_PostSpawnValidationFailed	= 6		UMETA(DisplayName = "Post Spawn Validation failed"),
	LGE_NoLayoutPresent				= 7		UMETA(DisplayName = "No layout present"),
	LGE_Unknown						= 255	UMETA(DisplayName = "Unknown"),
};

UENUM(BlueprintType)
enum class ELayoutGeneratorTasks : uint8
{
	LGT_None						= 0		UMETA(DisplayName = "None"),
	LGT_InitProperties				= 1		UMETA(DisplayName = "Initializing Properties"),
	LGT_GeneratingRooms				= 2		UMETA(DisplayName = "Generating Rooms"),
	LGT_RunningPostSpawnValidation	= 3		UMETA(DisplayName = "Running Post Spawn Validation"),
	LGT_LoadingLevels				= 4		UMETA(DisplayName = "Loading Levels"),
	LGT_Unknown						= 255	UMETA(DisplayName = "Unknown"),
};

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLayoutGeneratorTaskDone, bool, bSuccess, ELayoutGeneratorErrors, Error);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLayoutGeneratorTaskUpdate, float, PercentDone, ELayoutGeneratorTasks, CurrentTask);

UCLASS()
class SCPPU_API ALayoutGenerator_Main : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALayoutGenerator_Main();

	friend class ULayoutGenerator_Cell;

	//// FUNCTIONS ////

	/** Generates a new layout. Use the seed variable to set the seed. Ignores calls if a layout is already present. */
	UFUNCTION(BlueprintCallable)
		void AsyncGenerateLayout(const int32 NewSeed, bool bShowAllLevelsWhenDone);

	/** Clears the layout. Ignores calls if no layout is present. */
	UFUNCTION(BlueprintCallable)
		bool ClearLayout();

	UFUNCTION(BlueprintCallable)
		TArray<FIntVector2D> FindCellLocationsWithRoomRowName(const FName RoomRowName);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		ULayoutGenerator_Cell* GetCell(const FIntVector2D CellLocation);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Does Path Exists (Currently always returns false!)"))
		bool DoesPathExist(const FIntVector2D Start, const FIntVector2D End);

	UFUNCTION(BlueprintCallable)
		void DrawDebug(float Duration = 10000.f, bool bDrawCells = true);

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

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category = "Runtime")
		FLayoutGeneratorTaskDone OnLayoutGenerated;

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category = "Runtime")
		FLayoutGeneratorTaskUpdate OnLayoutGenerationStatusUpdate;

	UPROPERTY(VisibleAnywhere, BlueprintAssignable, Category = "Runtime")
		FLayoutGeneratorTaskDone OnLayoutCleared;

protected:
	/** Called when the game starts or when spawned */
	virtual void BeginPlay() override;

	/** Called every frame */
	virtual void Tick(float DeltaTime) override;

	/** Called whenever this actor is being removed from a level */
	virtual void EndPlay();

	//// FUNCTIONS ////

	bool InitRuntimeProperties(ELayoutGeneratorErrors& OutError);

	bool GenerateRequiredRooms(ELayoutGeneratorErrors& OutError);

	bool GenerateFromQueue(ELayoutGeneratorErrors& OutError);

	bool RunPostSpawnValidation(ELayoutGeneratorErrors& OutError);

	void ResetRuntimeProperties();

	UFUNCTION()
		void OnLevelLoadedCallback();

	//// PROPERTIES ////

	/** Random stream created from seed variable. */
	UPROPERTY(VisibleAnywhere, Category = "DEBUG")
		FRandomStream RandomStream;

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

	/** Amount of levels that are currently loading */
	UPROPERTY(VisibleAnywhere, Category = "DEBUG")
		int32 LevelsCurrentlyLoading;

	/** Cached spawn validator objects*/
	UPROPERTY(VisibleAnywhere, Category = "DEBUG")
		TMap<TSubclassOf<ULayoutGenerator_SpawnValidator>, ULayoutGenerator_SpawnValidator*> SpawnValidators;

	/** Contains the actual grid. */
	UPROPERTY(VisibleAnywhere, Category = "DEBUG")
		TMap<FIntVector2D, ULayoutGenerator_Cell*> Grid;

};
