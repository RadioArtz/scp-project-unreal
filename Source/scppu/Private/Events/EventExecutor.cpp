


#include "Events/EventExecutor.h"

// Sets default values
AEventExecutor::AEventExecutor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEventExecutor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEventExecutor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

