

#pragma once


#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Layout/LayoutSpawnValidator.h"
#include "LayoutStructs.generated.h"

USTRUCT(BlueprintType)
struct FIntVector2 
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 X;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 Y;

	FIntVector2()
	{
		this->X = 0;
		this->Y = 0;
	}

	FIntVector2(int32 InX, int32 InY)
	{
		this->X = InX;
		this->Y = InY;
	}

	friend bool operator==(const FIntVector2& Self, const FIntVector2& Other)
	{
		return Self.X == Other.X && Self.Y == Other.Y;
	}

	friend bool operator!=(const FIntVector2& Self, const FIntVector2& Other)
	{
		return Self.X != Other.X || Self.Y != Other.Y;
	}

	friend uint32 GetTypeHash(const FIntVector2& Self)
	{
		return FCrc::MemCrc32(&Self, sizeof(Self));
	}
};

USTRUCT(BlueprintType)
struct FLayoutCellSides
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bPX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bPY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bNX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool bNY;

	FLayoutCellSides()
	{
		this->bPX = false;
		this->bPY = false;
		this->bNX = false;
		this->bNY = false;
	}

	FLayoutCellSides(bool bInPX, bool bInPY, bool bInNX, bool bInNY)
	{
		this->bPX = bInPX;
		this->bPY = bInPY;
		this->bNX = bInNX;
		this->bNY = bInNY;
	}

	friend bool operator== (const FLayoutCellSides& Self, const FLayoutCellSides& Other)
	{
		return (Self.bPX == Other.bPX) && (Self.bPY == Other.bPY) && (Self.bNX == Other.bNX) && (Self.bNY == Other.bNY);
	}

	friend bool operator!= (const FLayoutCellSides& Self, const FLayoutCellSides& Other)
	{
		return (Self.bPX != Other.bPX) || (Self.bPY != Other.bPY) || (Self.bNX != Other.bNX) || (Self.bNY != Other.bNY);
	}

	void RotateRight()
	{
		FLayoutCellSides Prev = *this;
		this->bPX = Prev.bNY;
		this->bPY = Prev.bPX;
		this->bNX = Prev.bPY;
		this->bNY = Prev.bNX;
	}

	void RotateLeft()
	{
		FLayoutCellSides Prev = *this;
		this->bPX = Prev.bPY;
		this->bPY = Prev.bNX;
		this->bNX = Prev.bNY;
		this->bNY = Prev.bPX;
	}
};

USTRUCT(BlueprintType)
struct FLayoutCellGenerationSettings : public FTableRowBase
{
	GENERATED_BODY()

public:
	/* Diffrent level variations for more varaity (chooses one randomly).*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray< TSoftObjectPtr<UWorld> > Levels;

	/* Minimum instances that will be generated. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"))
		int32 RequiredInstances = 0;

	/* Maximum instances that will be generated. Must be >= 'Required Instances'. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "-1"))
		int32 MaximumInstances = -1;

	/* How present this row is inside the spawn pool (higher value => higer chance to generate). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1"))
		int32 SpawnPoolEntries = 10;

	/* The sides where the cell can connect to other cells. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLayoutCellSides HasConnections;

	/* The sides where the neighbouring cells are not alloweed to generate. Must not conflict with 'Has Connections'. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLayoutCellSides DisableNeighbouringCells;

	/* Validates a spawn location before setting the cell to this row. Not all cells have been generated during this time. They are AND gated. Must not contain 'None'. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray< TSubclassOf<ULayoutSpawnValidator> > PreSpawnValidators;

	/* Validates a spawn location after all cells have been generated. They are AND gated. Must not contain 'None'. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray< TSubclassOf<ULayoutSpawnValidator> > PostSpawnValidators;

	bool PassedSanityCheck()
	{
		bool bIsValid = true;
		bIsValid = bIsValid && (this->MaximumInstances < 0 || this->MaximumInstances >= this->RequiredInstances);
		bIsValid = bIsValid && (!this->DisableNeighbouringCells.bPX || (this->DisableNeighbouringCells.bPX && !this->HasConnections.bPX));
		bIsValid = bIsValid && (!this->DisableNeighbouringCells.bPY || (this->DisableNeighbouringCells.bPY && !this->HasConnections.bPY));
		bIsValid = bIsValid && (!this->DisableNeighbouringCells.bNX || (this->DisableNeighbouringCells.bNX && !this->HasConnections.bNX));
		bIsValid = bIsValid && (!this->DisableNeighbouringCells.bNY || (this->DisableNeighbouringCells.bNY && !this->HasConnections.bNY));

		for (auto Elem : this->PreSpawnValidators)
		{
			bIsValid = bIsValid && IsValid(Elem);
		}

		for (auto Elem : this->PostSpawnValidators)
		{
			bIsValid = bIsValid && IsValid(Elem);
		}

		return bIsValid;
	}
};