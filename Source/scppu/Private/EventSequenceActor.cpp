


#include "EventSequenceActor.h"

// Sets default values
AEventSequenceActor::AEventSequenceActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEventSequenceActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEventSequenceActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

