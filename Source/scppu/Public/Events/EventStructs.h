#pragma once

#include "CoreMinimal.h"
#include "EventStructs.generated.h"

UENUM()
enum class EGameEventType : uint8
{
	NormalEvent = 0,
	PeriodicEvent = 1
};

USTRUCT(BlueprintType)
struct FGameEvent
{
	GENERATED_BODY()

public:
	EGameEventType EventType = EGameEventType::NormalEvent;

	// All Events //
	UPROPERTY(BlueprintReadWrite)
		bool bOnlyExecuteOnce = false;

	UPROPERTY(BlueprintReadWrite)
		bool bAllowParallelExecution = false;

	UPROPERTY(BlueprintReadWrite)
		bool bCanBeCanceld = false;

	// Normal Events //
	UPROPERTY(BlueprintReadWrite)
		float ExecutionProbability = .5f;

	// Periodic Events //
	UPROPERTY(BlueprintReadWrite)
		bool bAutoActivate = false;

	UPROPERTY(BlueprintReadWrite)
		float MinTimeSeconds = 10.f;
	
	UPROPERTY(BlueprintReadWrite)
		float MaxTimeSeconds = 30.f;

};
