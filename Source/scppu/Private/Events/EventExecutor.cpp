#include "Events/EventExecutor.h"

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

