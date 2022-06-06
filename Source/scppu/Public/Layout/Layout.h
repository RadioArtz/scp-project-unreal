

#pragma once

#include "LayoutStructs.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Layout.generated.h"

class UDataTable;
class ULayoutCell;
class ULayoutSpawnValidator;

DECLARE_LOG_CATEGORY_EXTERN(LogLayout, Log, All);

UCLASS()
class SCPPU_API ALayout : public AActor
{
	GENERATED_BODY()
	
public:
	//// Properties ////
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
		bool bIsLayoutPresent; // get private set

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FIntVector2 GridSize; // get private set

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float CellSize; // get private set

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UDataTable* DataTable; // get private set

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
		FRandomStream RandStream; // get private set

	UPROPERTY(VisibleAnywhere, Category = "Debug")
		TMap<FIntVector2, ULayoutCell*> Grid; // get private set

	UPROPERTY(VisibleAnywhere, Category = "Debug") // private get private set
		TMap<TSubclassOf<ULayoutSpawnValidator>, ULayoutSpawnValidator*> SpawnValidatorCache;

	//// Functions ////
	// Sets default values for this actor's properties
	ALayout();

	UFUNCTION(BlueprintCallable)
		bool InitializeLayout(FIntVector2 NewGridSize, float NewCellSize, int32 NewSeed);

	UFUNCTION(BlueprintCallable)
		bool ClearLayout();

	UFUNCTION(BlueprintCallable)
		ULayoutSpawnValidator* GetOrCreateSpawnValidator(TSubclassOf<ULayoutSpawnValidator> InClass);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		ULayoutCell* GetCell(FIntVector2 Location);

	UFUNCTION(BlueprintCallable)
		void FindCellsWithRowName(FName RowName, TArray<ULayoutCell*>& OutCells);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		void GetNeighbouringCells(ULayoutCell* Origin, bool bOnlyReturnConnectedCells, ULayoutCell*& OutCellPX, ULayoutCell*& OutCellPY, ULayoutCell*& OutCellNX, ULayoutCell*& OutCellNY);

	UFUNCTION(BlueprintCallable)
		bool DoesPathExist(ULayoutCell* Start, ULayoutCell* Goal);

	UFUNCTION(BlueprintCallable)
		void LoadAllSublevels();

	UFUNCTION(BlueprintCallable)
		void UnloadAllSublevels();

	UFUNCTION(BlueprintCallable)
		void DrawDebug(float Duration = 5.0f, bool bDrawCells = true);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
