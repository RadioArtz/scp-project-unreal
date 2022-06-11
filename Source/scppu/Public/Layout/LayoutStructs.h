

#pragma once


#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "LayoutStructs.generated.h"

class ULayoutSpawnValidator;


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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray< TSoftObjectPtr<UWorld> > Levels;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"))
		int32 RequiredInstances = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "-1"))
		int32 MaximumInstances = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1"))
		int32 SpawnPoolEntries = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLayoutCellSides HasConnection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FLayoutCellSides DisableNeighbouringCell;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray< TSubclassOf<ULayoutSpawnValidator> > PreSpawnValidator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray< TSubclassOf<ULayoutSpawnValidator> > PostSpawnValidator;
};