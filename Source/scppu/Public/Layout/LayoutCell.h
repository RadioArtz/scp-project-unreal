

#pragma once

#include "LayoutStructs.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LayoutCell.generated.h"

class ALayout;
class ULevelStreamingDynamic;

UCLASS()
class SCPPU_API ULayoutCell : public UObject
{
	GENERATED_BODY()

public:
	//// Properties ////
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FIntVector2 Location;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int32 Rotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		ALayout* Owner;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bIsEnabled;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bIsGenerated;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FRandomStream RandStream;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FName RowName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TSoftObjectPtr<UWorld> LevelAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		ULevelStreamingDynamic* Sublevel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FString UniqueSublevelName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FLayoutCellSides HasConnection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FLayoutCellSides DisableNeighbouringCell;

	//// Functions ////
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FVector GetWorldLocation();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FRotator GetWorldRotation();

	UFUNCTION()
		bool SetRowName(FName NewRowName, bool bForce);

	UFUNCTION(BlueprintCallable)
		bool LoadSublevel();

	UFUNCTION(BlueprintCallable)
		bool UnloadSublevel();

	UFUNCTION(BlueprintCallable)
		void GetAllActorsInSublevel(TArray<AActor*>& OutActors);

	UFUNCTION(BlueprintCallable)
		bool IsPointInSublevelBounds(FVector Point);
	
	UFUNCTION(BlueprintCallable)
		void DrawDebug(float Duration = 5.0f);

protected:

	// Called when the game ends or when destroyed
	virtual void BeginDestroy() override;
};
