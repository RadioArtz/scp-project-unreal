


#include "InteractionComponent/InteractionComponentBase.h"

TArray<UInteractionComponentBase*> UInteractionComponentBase::RegisteredInteractionComponents;

TArray<UInteractionComponentBase*> UInteractionComponentBase::GetInteractionComponentsInRadius(FVector WorldLocation, float Radius)
{
	TArray<UInteractionComponentBase*> InteractionComponents;
	for (auto& Elem : UInteractionComponentBase::RegisteredInteractionComponents)
	{
		if (FVector::Distance(WorldLocation, Elem->GetComponentLocation()) <= Radius)
		{
			InteractionComponents.Add(Elem);
		}
	}

	return InteractionComponents;
}

UInteractionComponentBase* UInteractionComponentBase::GetNearestInteractionComponentInRadius(FVector WorldLocation, float Radius)
{
	UInteractionComponentBase* InteractionComponent = nullptr;
	for (auto& Elem : UInteractionComponentBase::RegisteredInteractionComponents)
	{
		if (FVector::Distance(WorldLocation, Elem->GetComponentLocation()) <= Radius)
		{
			if (IsValid(InteractionComponent))
			{
				if (FVector::Distance(WorldLocation, Elem->GetComponentLocation()) < FVector::Distance(WorldLocation, InteractionComponent->GetComponentLocation()))
				{
					InteractionComponent = Elem;
				}
			}
			else
			{
				InteractionComponent = Elem;
			}
		}
	}

	return InteractionComponent;
}

// Sets default values for this component's properties
UInteractionComponentBase::UInteractionComponentBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UInteractionComponentBase::BeginPlay()
{
	Super::BeginPlay();	
	UInteractionComponentBase::RegisteredInteractionComponents.Add(this);
}


// Called every frame
void UInteractionComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

