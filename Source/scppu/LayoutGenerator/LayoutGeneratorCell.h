

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LayoutGeneratorStructs.h"
#include "LayoutGeneratorCell.generated.h"

class ULevelStreamingDynamic;
class ALayoutGeneratorMain;

UINTERFACE(MinimalAPI)
class ULayoutGeneratorReceiveCellInterface : public UInterface
{
	GENERATED_BODY()
};

class ILayoutGeneratorReceiveCellInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, Category = "Layout Generator")
		void OnCellReceived(ULayoutGeneratorCell* Cell, FRandomStream UniqueRandomStream);
};

UCLASS()
class SCPPU_API ULayoutGeneratorCell : public UObject
{
	GENERATED_BODY()

public:

	friend class ALayoutGeneratorMain;

	//// FUNCTIONS ////

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FVector GetWorldLocation();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FRotator GetWorldRotation();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FString GetUniqueName();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		void GetNeighbouringCells(bool bOnlyReturnConnectedCells, ULayoutGeneratorCell*& OutCellPositiveX, ULayoutGeneratorCell*& OutCellPositiveY, ULayoutGeneratorCell*& OutCellNegativeX, ULayoutGeneratorCell*& OutCellNegativeY);

	UFUNCTION(BlueprintCallable)
		bool LoadLevel();

	UFUNCTION(BlueprintCallable)
		bool UnloadLevel();

	UFUNCTION(BlueprintCallable)
		void GetAllActorsInLevel(TArray<AActor*>& OutActors);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsPointInLevelBounds(FVector Point);

	UFUNCTION(BlueprintCallable)
		bool DoesPathExist(ULayoutGeneratorCell* Goal);

	UFUNCTION(BlueprintCallable)
		void DrawDebug(float Duration = 10000.f);

	//// PROPERTIES ////

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transform")
		FIntVector2D Location;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Transform", meta = (ClampMin = "0", ClampMax = "3"))
		int32 Rotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
		ALayoutGeneratorMain* LayoutGenerator;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
		TSoftObjectPtr<UWorld> LevelAsset;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "References")
		ULevelStreamingDynamic* LevelStreamingDynamic;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Properties")
		int32 UniqueSeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Properties")
		FName RoomRowName = "None";

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Properties")
		bool bIsGenerated = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Properties")
		bool bIsEnabled = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Properties")
		FLayoutGeneratorCellSides HasDoor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Properties")
		FLayoutGeneratorCellSides ShouldDisableNeighbour;

protected:
	virtual void BeginDestroy() override;

	//// FUNCTIONS ////

	UFUNCTION()
		bool SetRoom(const FName NewRoomRowName, const bool bForce);

	UFUNCTION()
		bool IsRoomValid();

	UFUNCTION()
		void OnLevelLoaded();
};
