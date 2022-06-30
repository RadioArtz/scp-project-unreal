


#include "InteractionComponent/InteractionComponentBase.h"

TArray<UInteractionComponentBase*> UInteractionComponentBase::RegisteredInteractionComponents;

TArray<UInteractionComponentBase*> UInteractionComponentBase::GetInteractionComponentsInRadius(FVector ClosestFrom, float Radius , bool bMustBeReachable, FVector ReachableFrom)
{
	TArray<UInteractionComponentBase*> InteractionComponents;
	for (auto& ElemComp : UInteractionComponentBase::RegisteredInteractionComponents)
	{
		if (FVector::Distance(ClosestFrom, ElemComp->GetComponentLocation()) <= Radius)
		{
			if (bMustBeReachable)
			{
				TArray<FHitResult> HitResults;
				FCollisionQueryParams CollisionParams;
				CollisionParams.TraceTag = FName("InteractionComponentSystem");
				CollisionParams.bTraceComplex = true;
				ElemComp->GetWorld()->LineTraceMultiByChannel(HitResults, ReachableFrom, ElemComp->GetComponentLocation(), ECollisionChannel::ECC_Visibility, CollisionParams);
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

void UInteractionComponentBase::StartInteraction(APawn* Interactor, UObject* Item)
{
	if (!this->bIsInUse)
	{
		this->bIsInUse = true;
		this->CurrentInteractor = Interactor;
		this->CurrentItem = Item;
	}
}

void UInteractionComponentBase::EndInteraction(APawn* Interactor)
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

