#include "Events/EventExecutor.h"

#include "Events/EventSubsystem.h"
#include "Events/GameEventAssets.h"

AGameEventExecutor::AGameEventExecutor()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AGameEventExecutor::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGameEventExecutor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool AGameEventExecutor::AllowExecution_Implementation(UBaseGameEvent* Event, UGameEventPayload* Payload, FTransform WorldTransform) const
{
	return false;
}