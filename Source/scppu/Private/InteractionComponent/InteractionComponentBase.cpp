


#include "InteractionComponent/InteractionComponentBase.h"

TArray<UInteractionComponentBase*> UInteractionComponentBase::RegisteredInteractionComponents;

TArray<UInteractionComponentBase*> UInteractionComponentBase::GetInteractionComponentsInRadius(FVector WorldLocation, float Radius, bool bMustBeReachable)
{
	TArray<UInteractionComponentBase*> InteractionComponents;
	for (auto& Elem : UInteractionComponentBase::RegisteredInteractionComponents)
	{
		if (FVector::Distance(WorldLocation, Elem->GetComponentLocation()) <= Radius)
		{
			if (bMustBeReachable)
			{
				FHitResult HitResult;
				FCollisionQueryParams CollisionParams;
				CollisionParams.AddIgnoredActor(Elem->GetOwner());
				Elem->GetWorld()->LineTraceSingleByChannel(HitResult, WorldLocation, Elem->GetComponentLocation(), ECollisionChannel::ECC_Visibility, CollisionParams);
				if (!HitResult.bBlockingHit)
				{
					InteractionComponents.Add(Elem);
				}
			}
			else
			{
				InteractionComponents.Add(Elem);
			}
		}
	}

	return InteractionComponents;
}

UInteractionComponentBase* UInteractionComponentBase::GetClosestInteractionComponentInRadius(FVector WorldLocation, float Radius, bool bMustBeReachable)
{
	UInteractionComponentBase* InteractionComponent = nullptr;
	for (auto& Elem : UInteractionComponentBase::GetInteractionComponentsInRadius(WorldLocation, Radius, bMustBeReachable))
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

	return InteractionComponent;
}

// Sets default values for this component's properties
UInteractionComponentBase::UInteractionComponentBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

void UInteractionComponentBase::StartInteraction(APawn* Interactor, UObject* Item)
{
	if (!this->bIsInUse)
	{
		this->bIsInUse = true;
		this->CurrentInteractor = Interactor;
		this->CurrentItem = Item;
	}
}

void UInteractionComponentBase::StopInteraction(APawn* Interactor)
{
	if (this->CurrentInteractor == Interactor)
	{
		this->bIsInUse = false;
		this->CurrentInteractor = nullptr;
		this->CurrentItem = nullptr;
	}
}

// Called when the game starts
void UInteractionComponentBase::BeginPlay()
{
	Super::BeginPlay();

	// Don't register components with no owner (e.g. default object)
	if (IsValid(this->GetOwner()))
	{
		UInteractionComponentBase::RegisteredInteractionComponents.Add(this);
	}
}

void UInteractionComponentBase::BeginDestroy()
{
	Super::BeginDestroy();
	UInteractionComponentBase::RegisteredInteractionComponents.Remove(this);
}

// Called every frame
void UInteractionComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

