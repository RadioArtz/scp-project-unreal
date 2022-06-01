

#pragma once

#include "LayoutStructs.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Layout.generated.h"

class UDataTable;
class ULayoutCell;

DECLARE_LOG_CATEGORY_EXTERN(LogLayout, Log, All);

UCLASS()
class SCPPU_API ALayout : public AActor
{
	GENERATED_BODY()
	
public:
	//// Properties ////
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
		bool bIsLayoutPresent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FIntVector2 GridSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float CellSize;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UDataTable* DataTable;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
		FRandomStream RandStream;

	UPROPERTY(VisibleAnywhere, Category = "Debug")
		TMap<FIntVector2, ULayoutCell*> Grid;

	//// Functions ////

	// Sets default values for this actor's properties
	ALayout();

	UFUNCTION(BlueprintCallable)
		bool NewLayout(FIntVector2 NewGridSize, float NewCellSize, int32 NewSeed);

	UFUNCTION(BlueprintCallable)
		bool ClearLayout();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		ULayoutCell* GetCell(FIntVector2 Location);

	UFUNCTION(BlueprintCallable)
		void FindCellsWithRowName(FName RowName, TArray<ULayoutCell*>& OutCells);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		void GetNeighbouringCells(ULayoutCell* Origin, bool bOnlyReturnConnectedCells, ULayoutCell*& OutCellPX, ULayoutCell*& OutCellPY, ULayoutCell*& OutCellNX, ULayoutCell*& OutCellNY);

	UFUNCTION(BlueprintCallable)
		bool DoesPathExist(ULayoutCell* Start, ULayoutCell* Target);

	UFUNCTION(BlueprintCallable)
		void LoadAllSublevels();

	UFUNCTION(BlueprintCallable)
		void UnloadAllSublevels();

	UFUNCTION(BlueprintCallable)
		void DrawDebug(float Duration = 5.0f, bool bDrawCellContent = true);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
