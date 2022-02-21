

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "LayoutGenerator_Structs.generated.h"

class ULayoutGenerator_SpawnValidator;

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
	}

	FIntVector2D(int32 NewX, int32 NewY)
	{
		X = NewX;
		Y = NewY;
	}

	// Needed to be able to use this struct as key inside a map
	friend bool operator== (const FIntVector2D& Self, const FIntVector2D& Other)
	{
		return Self.X == Other.X && Self.Y == Other.Y;
	}

	friend uint32 GetTypeHash(const FIntVector2D& Other)
	{
		FString StringX = FString::FromInt(Other.X);
		FString StringY = FString::FromInt(Other.Y);
		return FCString::Atoi(*(StringX += StringY));
	}
};

USTRUCT(BlueprintType)
struct FCellSides
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
};

USTRUCT(BlueprintType)
struct FRoomGenerationSettings : public FTableRowBase
{
	GENERATED_BODY()

public:
	/** Maps of the room (if > 1: will choose one randomly per cell, should be only used for minor visual variations).*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray< TSoftObjectPtr<UWorld> > Maps;

	/** Minimum instances that will be generated. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"))
		int32 MinimumInstances = 0;

	/** Maximum instances that can generate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0"))
		int32 MaximumInstances = 1000;

	/** Amount of entries inside the spawn pool. Higher values => Higher probability to generate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "1"))
		int32 SpawnPoolAmount = 10;

	/** The directions of the door location. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FCellSides DoorLocation;

	/** The directions of which neighbour to disable. Usefull when having a room that exceeds the cell size. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FCellSides DisableNeighbour;

	/** 
	*Validates a spawn location before spawning the room (NOTE: Not all rooms have been spawned yet!).
	*Return value gets AND gated.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray< TSubclassOf<ULayoutGenerator_SpawnValidator> > PreSpawnValidator;
	
	/** 
	*Validates a spawn location after spawning all rooms.
	*Return value gets AND gated.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray< TSubclassOf<ULayoutGenerator_SpawnValidator> > PostSpawnValidator;
};

USTRUCT(BlueprintType)
struct FGridCell
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FName RoomRowName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bIsGenerated = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool bIsEnabled = true;

	// Will rotate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FCellSides DoorLocation;

	// Will rotate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FCellSides DisableNeighbour;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FCellSides DoorRequired;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FCellSides DoorBlocked;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (ClampMin = "0", ClampMax = "3"))
		int32 Rotation;
};