#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameEventAssets.generated.h"

class AGameEventExecutor;

UCLASS(Abstract)
class SCPPU_API UBaseGameEvent : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOnlyExecuteOnce = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAllowParallelExecution = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanBeCanceled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AGameEventExecutor> EventExecutor;
};

UCLASS(Blueprintable)
class SCPPU_API UNormalGameEvent : public UBaseGameEvent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ExecutionProbability = .5f;

};

UCLASS(Blueprintable)
class SCPPU_API UPeriodicGameEvent : public UBaseGameEvent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinTimeSeconds = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxTimeSeconds = 30.f;

};