


#include "InteractionComponents/BaseInteractionComponent.h"

class ABaseItem;

TArray<UBaseInteractionComponent*> UBaseInteractionComponent::RegisteredInteractionComponents;

TArray<UBaseInteractionComponent*> UBaseInteractionComponent::GetInteractionComponentsInRadius(FVector ClosestFrom, float Radius , bool bMustBeReachable, FVector ReachableFrom)
{
	TArray<UBaseInteractionComponent*> InteractionComponents;
	for (auto& ElemComp : UBaseInteractionComponent::RegisteredInteractionComponents)
	{
		if (ElemComp->bIsEnabled && FVector::Distance(ClosestFrom, ElemComp->GetComponentLocation()) <= Radius)
		{
			if (bMustBeReachable)
			{
				TArray<FHitResult> HitResults;
				FCollisionQueryParams CollisionParams;
				CollisionParams.TraceTag = FName("InteractionComponentSystem");
				CollisionParams.bTraceComplex = true;
				ElemComp->GetWorld()->LineTraceMultiByChannel(HitResults, ElemComp->GetComponentLocation(), ReachableFrom, ECollisionChannel::ECC_Visibility, CollisionParams);
				bool bAnyValidBlockingHits = false;
				for (auto& ElemHit : HitResults)
				{
					if (ElemHit.IsValidBlockingHit())
					{
						bAnyValidBlockingHits = true;
					}
				}
				
				if (!bAnyValidBlockingHits)
				{
					InteractionComponents.Add(ElemComp);
				}
			}
			else
			{
				InteractionComponents.Add(ElemComp);
			}
		}
	}

	return InteractionComponents;
}

UBaseInteractionComponent* UBaseInteractionComponent::GetClosestInteractionComponentInRadius(FVector ClosestFrom, float Radius, bool bMustBeReachable, FVector ReachableFrom)
{
	UBaseInteractionComponent* InteractionComponent = nullptr;
	for (auto& Elem : UBaseInteractionComponent::GetInteractionComponentsInRadius(ClosestFrom, Radius, bMustBeReachable, ReachableFrom))
	{
		// (ignore warning, IsValid() checks for nullptr)
		if (IsValid(InteractionComponent))
		{
			if (FVector::Distance(ClosestFrom, Elem->GetComponentLocation()) < FVector::Distance(ClosestFrom, InteractionComponent->GetComponentLocation()))
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
UBaseInteractionComponent::UBaseInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	this->PrimaryComponentTick.bCanEverTick = true;
	this->PrimaryComponentTick.bStartWithTickEnabled = false;
}

bool UBaseInteractionComponent::StartInteraction(APawn* Interactor, ABaseItem* Item)
{
	if (!this->bIsEnabled || this->bIsInUse || Interactor == nullptr)
	{
		return false;
	}

	this->bIsInUse = true;
	this->CurrentInteractor = Interactor;
	this->CurrentItem = Item;
	return true;
}

bool UBaseInteractionComponent::EndInteraction(APawn* Interactor)
{
	if (!this->bIsEnabled || !this->bIsInUse || this->CurrentInteractor != Interactor)
	{
		return false;
	}

	this->bIsInUse = false;
	this->CurrentInteractor = nullptr;
	this->CurrentItem = nullptr;
	return true;
}

void UBaseInteractionComponent::SetEnabled(bool bNewEnabled)
{
	if (!bNewEnabled)
	{
		this->EndInteraction(this->CurrentInteractor);
	}

	this->bIsEnabled = bNewEnabled;
}

// Called when the game starts
void UBaseInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// Don't register components with no owner (e.g. default object)
	if (IsValid(this->GetOwner()))
	{
		UBaseInteractionComponent::RegisteredInteractionComponents.Add(this);
	}
}

void UBaseInteractionComponent::BeginDestroy()
{
	Super::BeginDestroy();
	UBaseInteractionComponent::RegisteredInteractionComponents.Remove(this);
}

// Called every frame
void UBaseInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

