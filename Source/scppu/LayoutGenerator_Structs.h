

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "LayoutGenerator_Structs.generated.h"

class ULevelStreaming;
class ULayoutGenerator_SpawnValidator;
class ALayoutGenerator_RoomSpawner;
class ABaseRoom;

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

	// Needed to use this struct as key inside a map
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
struct FGridEntry
{
	GENERATED_BODY()

public:
	UPROPERTY()
		ALayoutGenerator_RoomSpawner* RoomSpawner;

	UPROPERTY()
		ULevelStreaming* LevelStreaming;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		ABaseRoom* Room;
};

USTRUCT(BlueprintType)
struct FRoomSides
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool PositiveX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool PositiveY;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool NegativeX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool NegativeY;
};

USTRUCT(BlueprintType)
struct FRoomGenerationSettings : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftObjectPtr<UWorld> Level;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 MinimumInstances;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 MaximumInstances;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 SpawnPoolAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRoomSides DoorLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRoomSides DisableNeighbour;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray< TSubclassOf<ULayoutGenerator_SpawnValidator> > PreSpawnValidator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TArray< TSubclassOf<ULayoutGenerator_SpawnValidator> > PostSpawnValidator;
};