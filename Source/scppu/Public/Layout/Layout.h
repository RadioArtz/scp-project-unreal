

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Layout/LayoutStructs.h"
#include "Layout.generated.h"

class USceneComponent;
class UBillboardComponent;
class UDataTable;
class ULayoutCell;
class ULayoutSpawnValidator;

DECLARE_LOG_CATEGORY_EXTERN(LogLayout, Log, All);

UCLASS(BlueprintType)
class SCPPU_API ALayout : public AActor
{
	GENERATED_BODY()
	
	//// Properties ////
public:
	static TArray<ALayout*> ActiveLayouts;

	UPROPERTY()
		USceneComponent* Root;

	UPROPERTY()
		UBillboardComponent* Sprite;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bIsLayoutPresent; // get private set

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
		bool bIsReadOnly; // get set

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FIntVector2 GridSize; // get private set

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float CellSize; // get private set

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UDataTable* DataTable; // get private set

	UPROPERTY(VisibleAnywhere, Category = "Debug")
		FRandomStream RStream; // get private set

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TMap<FIntVector2, ULayoutCell*> Grid; // get private set

	//// Functions ////
public:
	UFUNCTION(BlueprintCallable)
		static void FindLayoutAndCellFromWorldLocation(ALayout*& OutLayout, ULayoutCell*& OutCell, FVector WorldLocation, float ZTolerance = -1.0f);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static FORCEINLINE TArray<ALayout*> GetActiveLayouts() { return ActiveLayouts; }

	// Sets default values for this actor's properties
	ALayout();

	UFUNCTION(BlueprintCallable)
		bool SetConfig(FIntVector2 NewGridSize, float NewCellSize, UDataTable* NewDataTable);

	UFUNCTION(BlueprintCallable)
		bool Initialize(int32 NewSeed);

	UFUNCTION(BlueprintCallable)
		bool Clear();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE int GetSeed() { return this->RStream.GetInitialSeed(); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE ULayoutCell* GetCell(FIntVector2 Location) { return this->Grid.FindRef(Location); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		ULayoutCell* FindCellFromWorldLocation(FVector WorldLocation, float ZTolerance = -1.0f);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		void GetNeighbouringCells(ULayoutCell* Origin, bool bOnlyReturnConnectedCells, ULayoutCell*& OutCellPX, ULayoutCell*& OutCellPY, ULayoutCell*& OutCellNX, ULayoutCell*& OutCellNY);

	UFUNCTION(BlueprintCallable)
		ULayoutCell* FindCellWithRowName(FName RowName);

	UFUNCTION(BlueprintCallable)
		void FindCellsWithRowName(FName RowName, TArray<ULayoutCell*>& OutCells);
	
	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = CellsToAvoid, AdvancedDisplay = CellsToAvoid))
		bool DoesPathExist(ULayoutCell* Start, ULayoutCell* Goal, TArray<ULayoutCell*> CellsToAvoid);

	UFUNCTION(BlueprintCallable)
		void LoadAllSublevels(bool bShowAllSublevels);

	UFUNCTION(BlueprintCallable)
		void UnloadAllSublevels(bool bForce);

	UFUNCTION(BlueprintCallable)
		void ShowAllSublevels();

	UFUNCTION(BlueprintCallable)
		void HideAllSublevels(bool bForce);

	UFUNCTION(BlueprintCallable)
		void DrawDebug(float Duration = 5.0f, bool bDrawCells = true, bool bShowText = true);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
};
