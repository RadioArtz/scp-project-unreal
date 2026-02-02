#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EventExecutor.generated.h"

class UBaseGameEvent;
class UGameEventPayload;

UCLASS()
class SCPPU_API AGameEventExecutor : public AActor
{
	GENERATED_BODY()
	
public:	
	AGameEventExecutor();

	UFUNCTION(BlueprintNativeEvent)
	bool AllowExecution(UBaseGameEvent* Event, UGameEventPayload* Payload, FTransform WorldTransform) const;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
