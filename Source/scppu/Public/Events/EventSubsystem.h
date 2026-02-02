#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "EventStructs.h"
#include "EventSubsystem.generated.h"

class AGameEventExecutor;

class UBaseGameEvent;
class UNormalGameEvent;
class UPeriodicGameEvent;


USTRUCT(BlueprintType)
struct FGameEventHandle
{
	GENERATED_BODY();

public:
	uint64 Id;
};

UCLASS(Blueprintable)
class SCPPU_API UGameEventPayload : public UObject
{
	GENERATED_BODY()
};

UENUM(BlueprintType)
enum class EGameEventStatus : uint8
{
	Invalid = 0,
	Queued = 1,
	Executing = 2,
	Finished = 3,
	Canceled = 4
};

UCLASS()
class SCPPU_API UGameEventSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void Initialize();

	UFUNCTION(BlueprintCallable)
	void Deinitialize();

	UFUNCTION(BlueprintCallable)
	FGameEventHandle ExecuteNormalEvent(UNormalGameEvent* EventAsset, UGameEventPayload* Payload, FTransform WorldTransform);

	UFUNCTION(BlueprintCallable)
	FGameEventHandle ExecutePeriodicEvent(UPeriodicGameEvent* EventAsset, UGameEventPayload* Payload);

	UFUNCTION(BlueprintCallable)
	AGameEventExecutor* GetEventExecutor(FGameEventHandle Handle) const;

	UFUNCTION(BlueprintCallable)
	EGameEventStatus GetEventStatus(FGameEventHandle Handle) const;

	UFUNCTION(BlueprintCallable)
	UGameEventPayload* GetEventPayload(FGameEventHandle Handle) const;

	UFUNCTION(BlueprintCallable)
	void SetEventPayload(FGameEventHandle Handle, UGameEventPayload* NewPayload);

	UFUNCTION(BlueprintCallable)
	UBaseGameEvent* GetEventAsset(FGameEventHandle Handle) const;

	UFUNCTION(BlueprintCallable)
	void CancelEvent(FGameEventHandle Handle, bool bForceCancel);

private:
	uint64 NextId = 0;
};
