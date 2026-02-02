#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EventExecutor.generated.h"

UCLASS()
class SCPPU_API AGameEventExecutor : public AActor
{
	GENERATED_BODY()
	
public:	
	AGameEventExecutor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

};
