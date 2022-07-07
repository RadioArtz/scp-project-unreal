


#include "InteractionComponents/InteractionComponentBase.h"

class AItemBase;

TArray<UInteractionComponentBase*> UInteractionComponentBase::RegisteredInteractionComponents;

TArray<UInteractionComponentBase*> UInteractionComponentBase::GetInteractionComponentsInRadius(FVector ClosestFrom, float Radius , bool bMustBeReachable, FVector ReachableFrom)
{
	TArray<UInteractionComponentBase*> InteractionComponents;
	for (auto& ElemComp : UInteractionComponentBase::RegisteredInteractionComponents)
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

UInteractionComponentBase* UInteractionComponentBase::GetClosestInteractionComponentInRadius(FVector ClosestFrom, float Radius, bool bMustBeReachable, FVector ReachableFrom)
{
	UInteractionComponentBase* InteractionComponent = nullptr;
	for (auto& Elem : UInteractionComponentBase::GetInteractionComponentsInRadius(ClosestFrom, Radius, bMustBeReachable, ReachableFrom))
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
UInteractionComponentBase::UInteractionComponentBase()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	this->PrimaryComponentTick.bCanEverTick = true;
	this->PrimaryComponentTick.bStartWithTickEnabled = false;
}

bool UInteractionComponentBase::StartInteraction(APawn* Interactor, AItemBase* Item)
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

bool UInteractionComponentBase::EndInteraction(APawn* Interactor)
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

void UInteractionComponentBase::SetIsEnabled(bool bNewIsEnabled)
{
	if (!bNewIsEnabled)
	{
		this->EndInteraction(this->CurrentInteractor);
	}

	this->bIsEnabled = bNewIsEnabled;
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

