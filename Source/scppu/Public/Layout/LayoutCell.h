

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
		FIntVector2 Location; // get private set

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int32 Rotation; // get private set

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		ALayout* Owner; // get private set

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bIsGenerated; // get private set

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int32 UniqueSeed; // get private set

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FName RowName; // get private set

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TSoftObjectPtr<UWorld> LevelAsset; // get private set

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		ULevelStreamingDynamic* Sublevel; // get private set

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FLayoutCellSides HasConnections; // get private set

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FLayoutCellSides DisableNeighbouringCells; // get private set

	//// Functions ////
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FVector GetWorldLocation(); // move to header file to make inline work

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FRotator GetWorldRotation(); // move to header file to make inline work

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FLayoutCellSides GetRequiredConnections(); // move to header file to make inline work

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FLayoutCellSides GetBlockedConnections(); // move to header file to make inline work

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FString GetUniqueSublevelName(); // move to header file to make inline work

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsBlockedByNeighbour(); // move to header file to make inline work

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsRequiredToGenerate();

	UFUNCTION(BlueprintCallable)
		bool IsRowNameValid(FName InRowName, int InRotation);

	UFUNCTION(BlueprintCallable)
		void SetRowName(FName NewRowName, int NewRotation);

	UFUNCTION(BlueprintCallable)
		void LoadSublevel();

	UFUNCTION(BlueprintCallable)
		void UnloadSublevel();

	UFUNCTION()
		void OnSublevelLoadedCallback();

	UFUNCTION(BlueprintCallable)
		void GetAllActorsOfClassInSublevel(TSubclassOf<AActor> ActorClass, TArray<AActor*>& OutActors);

	UFUNCTION(BlueprintCallable)
		bool IsPointInSublevelBounds(FVector Point);
	
	UFUNCTION(BlueprintCallable)
		void DrawDebug(float Duration = 5.0f, bool bShowText = true);

protected:

	// Called when the game ends or when destroyed
	virtual void BeginDestroy() override;
};
