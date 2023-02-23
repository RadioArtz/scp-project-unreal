

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Utilities/ExtendedPrecomputedLightVolume.h"
#include "Layout/Layout.h"
#include "Layout/LayoutStructs.h"
#include "LayoutCell.generated.h"

class ULevelStreamingDynamic;

UCLASS(BlueprintType, Within="Layout")
class SCPPU_API ULayoutCell : public UObject
{
	GENERATED_BODY()

	//// Properties ////
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FIntVector2 Location; // get private set

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		int32 Rotation; // get private set

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bLevelAlwaysVisible; // get private set

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bLevelAlwaysLoaded; // get private set

	//// Functions ////
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE ALayout* GetLayout() { return Cast<ALayout>(this->GetOuter()); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE FVector GetWorldLocation() { return (FVector(this->Location.X, this->Location.Y, 0) * this->GetLayout()->CellSize) + FVector(this->GetLayout()->CellSize / 2, this->GetLayout()->CellSize / 2, 0) + this->GetLayout()->GetActorLocation(); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE FRotator GetWorldRotation() { return FRotator(0, this->Rotation * 90, 0); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FLayoutCellSides GetRequiredConnections();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FLayoutCellSides GetBlockedConnections();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE FString GetSublevelName() { return this->GetName() + "_Sublevel"; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsBlockedByNeighbour();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE bool IsRequiredToGenerate() { return this->GetRequiredConnections() != FLayoutCellSides(false, false, false, false); }

	UFUNCTION(BlueprintCallable)
		bool IsRowNameValid(FName InRowName, int InRotation);

	UFUNCTION(BlueprintCallable)
		void SetRowName(FName NewRowName, int NewRotation);

	UFUNCTION(BlueprintCallable)
		void ResetRowName();

	UFUNCTION(BlueprintCallable)
		void LoadSublevel(bool bShowSublevel);

	UFUNCTION(BlueprintCallable)
		void UnloadSublevel(bool bForce);

	UFUNCTION(BlueprintCallable)
		void ShowSublevel();

	UFUNCTION(BlueprintCallable)
		void HideSublevel(bool bForce);

	UFUNCTION()
		void OnSublevelLoadedCallback();

	UFUNCTION()
		void OnSublevelShownCallback();

	UFUNCTION(BlueprintCallable)
		void GetAllActorsOfClassInSublevel(TSubclassOf<AActor> ActorClass, TArray<AActor*>& OutActors);

	UFUNCTION(BlueprintCallable, meta = (DeprecatedFunction))
		bool TransferSublevelActorToPresistentLevel(AActor* Actor);

	UFUNCTION(BlueprintCallable, meta = (DeprecatedFunction))
		bool TransferPresistentLevelActorToSublevel(AActor* Actor);

	UFUNCTION(BlueprintCallable)
		bool IsPointInSublevelBounds(FVector Point);
	
	UFUNCTION(BlueprintCallable)
		void DrawDebug(float Duration = 5.0f, bool bShowText = true);

protected:

	// Called when the game ends or when destroyed
	virtual void BeginDestroy() override;
};
