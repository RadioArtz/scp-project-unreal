

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
		FString StringX = FString::FromInt(Self.X);
		FString StringY = FString::FromInt(Self.Y);
		return FCString::Atoi(*(StringX += StringY));
	}
};

USTRUCT(BlueprintType)
struct FLayoutCellSides
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool PX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool PY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool NX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool NY;
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