

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "LayoutGeneratorStructs.generated.h"

class ULayoutGeneratorSpawnValidator;

USTRUCT(BlueprintType)
struct FIntVector2D
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Y;

	FIntVector2D()
	{
		X = 0;
		Y = 0;
	}

	FIntVector2D(int32 NewX, int32 NewY)
	{
		X = NewX;
		Y = NewY;
	}

	// Needed to use this struct as key inside a map
	friend bool operator== (const FIntVector2D& Self, const FIntVector2D& Other)
	{
		return Self.X == Other.X && Self.Y == Other.Y;
	}

	// Needed to use this struct as key inside a map
	friend uint32 GetTypeHash(const FIntVector2D& Other)
	{
		FString StringX = FString::FromInt(Other.X);
		FString StringY = FString::FromInt(Other.Y);
		return FCString::Atoi(*(StringX += StringY));
	}
};

USTRUCT(BlueprintType)
struct FLayoutGeneratorCellSides
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bPositiveX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bPositiveY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bNegativeX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bNegativeY;

	FLayoutGeneratorCellSides()
	{
		bPositiveX = false;
		bPositiveY = false;
		bNegativeX = false;
		bNegativeY = false;
	}

	FLayoutGeneratorCellSides(bool bNewPosX, bool bNewPosY, bool bNewNegX, bool bNewNegY)
	{
		bPositiveX = bNewPosX;
		bPositiveY = bNewPosY;
		bNegativeX = bNewNegX;
		bNegativeY = bNewNegY;
	}

	friend bool operator== (const FLayoutGeneratorCellSides& Self, const FLayoutGeneratorCellSides& Other)
	{
		return (Self.bPositiveX == Other.bPositiveX) && (Self.bPositiveY == Other.bPositiveY) && (Self.bNegativeX == Other.bNegativeX) && (Self.bNegativeY == Other.bNegativeY);
	}

	void RotateRight()
	{
		FLayoutGeneratorCellSides Original;

		Original = *this;
		this->bPositiveX = Original.bNegativeY;
		this->bPositiveY = Original.bPositiveX;
		this->bNegativeX = Original.bPositiveY;
		this->bNegativeY = Original.bNegativeX;

		return;
	}

	void RotateLeft()
	{
		FLayoutGeneratorCellSides Original;
	
		Original = *this;
		this->bPositiveX = Original.bPositiveY;
		this->bPositiveY = Original.bNegativeX;
		this->bNegativeX = Original.bNegativeY;
		this->bNegativeY = Original.bPositiveX;

		return;
	}
};

USTRUCT(BlueprintType)
struct FLayoutGeneratorRoomGenerationSettings : public FTableRowBase
{
	GENERATED_BODY()

public:
	/** Diffrent levels of the room (if > 1: chooses one level randomly per cell, should only be done for minor visual variations).*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray< TSoftObjectPtr<UWorld> > Levels;

	/** How many instances are required to get generated. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"))
		int32 RequiredInstances = 0;

	/** How many instances at maximum can get generated. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"))
		int32 MaximumInstances = 1000;

	/** Amount of entries inside the spawn pool. Higher values => Higher probability to generate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1"))
		int32 SpawnPoolAmount = 10;

	/** The locations of the doors. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLayoutGeneratorCellSides HasDoor;

	/** Which neighbouring cell to disable. Usefull when having a room that exceeds the cell bounds. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLayoutGeneratorCellSides ShouldDisableNeighbour;

	/** 
	*Validates a spawn location before generating the room (NOTE: Not all rooms have been generated yet!).
	*Return value gets AND gated.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray< TSubclassOf<ULayoutGeneratorSpawnValidator> > PreSpawnValidator;
	
	/** 
	*Validates a spawn location after generating all rooms.
	*Return value gets AND gated.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray< TSubclassOf<ULayoutGeneratorSpawnValidator> > PostSpawnValidator;
};