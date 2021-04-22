// 


#include "PlayerInteractionBase.h"


#include "AIConfig.h"
#include "Engine/World.h"
#include "scppu/Interaction/InteractionGrab.h"
#include "scppu/Interaction/InteractionSimple.h"


void FPlayerInteractionBaseSecondaryTickFunction::ExecuteTick(float DeltaTime, ELevelTick TickType,
	ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent)
{
	ExecuteTickHelper(Target, Target->bTickInEditor, DeltaTime, TickType, [this, TickType](float DilatedTime)
    {
        const auto PTarget = Cast<UPlayerInteractionBase>(Target);
		if(PTarget)
		{
			PTarget->SecondaryTickComponent(DilatedTime, TickType, this);
		}
    });
}

// Sets default values for this component's properties
UPlayerInteractionBase::UPlayerInteractionBase()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.2;
	PrimaryComponentTick.TickGroup = TG_DuringPhysics;
	PrimaryComponentTick.bStartWithTickEnabled = true;

	SecondaryComponentTick.Target = this;
	SecondaryComponentTick.bCanEverTick = true;
	SecondaryComponentTick.TickInterval = 0;
	SecondaryComponentTick.TickGroup = TG_DuringPhysics;
}

void UPlayerInteractionBase::BeginPlay()
{
	Super::BeginPlay();

	SecondaryComponentTick.RegisterTickFunction(GetComponentLevel());
	SecondaryComponentTick.SetTickFunctionEnable(false);
	
	const auto GameState = GetWorld()->GetGameState<AScpPuGameStateBase>();

	if(GameState)
	{
		ScpPuGameState = GameState;
	}
}

void UPlayerInteractionBase::TickComponent(float DeltaTime, ELevelTick TickType,
                                           FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(!bInteractionSearchLocked)
	{
		InternalSearchForNearestInteractiveObject();
	}
}

void UPlayerInteractionBase::SecondaryTickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
	if(CurrentlyGrabbedObject)
	{
		const auto& GObjLoc = CurrentlyGrabbedObject->GetOwner()->GetActorLocation();
		const auto& GObjDist = FVector::Dist(GetOwner()->GetActorLocation(), GObjLoc);
	
		if(GrabRange < GObjDist)
		{
			InteractionDrop();
		}
	}
	else if(bInteractionSearchLocked)
	{
		const auto& IObjDist = FVector::Dist(GetOwner()->GetActorLocation(), ClosestInteractiveObjectInfo.Location);

		if(InteractionRange < IObjDist)
		{
			// interaction has to end when player leaves interaction range, but searching for new objects should be disabled
			// until player releases their mouse button.
			InteractionEnd();
			bInteractionSearchLocked = true;

			ClosestInteractiveObject = nullptr;
			ClosestInteractiveObjectInfo = FInteractiveObjectInfo();
		}
	}
}

UInteractionBase* UPlayerInteractionBase::GetClosestInteractiveObject() const
{
	if(ClosestInteractiveObject.IsValid())
	{
		const auto& ClosestIObjLoc = ClosestInteractiveObject.Get()->GetOwner()->GetActorLocation();
		const FVector& OwnerLoc =  GetOwner()->GetActorLocation();
		
		if(FVector::Dist(ClosestIObjLoc, OwnerLoc) <= InteractionRange)
		{
			return ClosestInteractiveObject.Get();
		}
	}
	
	return nullptr;
}

FInteractiveObjectInfo UPlayerInteractionBase::GetClosestInteractiveObjectInfo() const
{
	return ClosestInteractiveObjectInfo;
}

UInteractionGrab* UPlayerInteractionBase::GetCurrentlyGrabbedObject() const
{
	return CurrentlyGrabbedObject;
}

bool UPlayerInteractionBase::CanInteractWithObject(const UInteractionBase* Obj) const
{
	if(!Obj)
	{
		return false;
	}

	if(CurrentlyGrabbedObject)
	{
		return false;
	}

	return true;
}

bool UPlayerInteractionBase::CanGrabObject(const UInteractionBase* Obj) const
{
	if(CurrentlyGrabbedObject)
	{
		return false;
	}

	const auto& ObjectToGrab = Cast<UInteractionGrab>(Obj);

	if(!ObjectToGrab)
	{
		return false;
	}

	const float& NewGrabRange = CalculateObjectGrabRange(ObjectToGrab);

	const auto& ObjectToGrabLoc = ObjectToGrab->GetOwner()->GetActorLocation();
	const auto& ObjectToGrabDist = FVector::Dist(GetOwner()->GetActorLocation(), ObjectToGrabLoc);
	
	if(NewGrabRange < ObjectToGrabDist)
	{
		return false;
	}

	return true;
}

bool UPlayerInteractionBase::InteractionStart()
{
	const auto& IObj = GetClosestInteractiveObject();

	if(!CanInteractWithObject(IObj))
	{
		return false;
	}

	switch (IObj->GetInteractionType())
	{
	case EInteractionType::TriggerOnStart:
	case EInteractionType::TriggerEverytime:
		{
			const auto ISimpleObj = Cast<UInteractionSimple>(IObj);
			ISimpleObj->Interact(this);
		}
		break;
		
	case EInteractionType::InteractionGrab:
		{
			const auto IGrabObj = Cast<UInteractionGrab>(IObj);
			InteractionGrab(IGrabObj);
		}
		break;
	

	default: ;
	}

	SecondaryComponentTick.SetTickFunctionEnable(true);
	bInteractionSearchLocked = true;
	
	return true;
}

bool UPlayerInteractionBase::InteractionEnd()
{
	bInteractionSearchLocked = false;
	
	const auto& IObj = ClosestInteractiveObject.Get();

	if(!IObj)
	{
		return false;
	}

	if(!InteractionDrop())
	{
		switch (IObj->GetInteractionType())
		{
		case EInteractionType::TriggerOnEnd:
        case EInteractionType::TriggerEverytime:
			{
				const auto ISimpleObj = Cast<UInteractionSimple>(IObj);
				ISimpleObj->Interact(this);
			}	
			break;
		
		default: ;
		}
	}

	SecondaryComponentTick.SetTickFunctionEnable(false);
	
	return true;
}

bool UPlayerInteractionBase::InteractionGrab(UInteractionGrab* ObjectToGrab)
{
	if(!CanGrabObject(ObjectToGrab))
	{
		return false;
	}

	if(ObjectToGrab->CustomGrabRange >= 0)
	{
		GrabRange = ObjectToGrab->CustomGrabRange;
	}
	else if(ObjectToGrab->CustomGrabRangeMultiplier >= 0)
	{
		GrabRange = DefaultGrabRange * ObjectToGrab->CustomGrabRangeMultiplier;
	} 
	
	ObjectToGrab->InteractGrab(this);
	CurrentlyGrabbedObject = ObjectToGrab;

	return true;
}

bool UPlayerInteractionBase::InteractionDrop()
{
	if(!CurrentlyGrabbedObject)
	{
		return false;
	}

	GrabRange = DefaultGrabRange;

	CurrentlyGrabbedObject->InteractDrop(this);
	CurrentlyGrabbedObject = nullptr;

	return true;
}

float UPlayerInteractionBase::CalculateObjectGrabRange(const UInteractionGrab* ObjectToGrab) const
{
	float NewGrabRange = GrabRange;
	
	if(ObjectToGrab->CustomGrabRange >= 0)
	{
		NewGrabRange = ObjectToGrab->CustomGrabRange;
	}
	else if(ObjectToGrab->CustomGrabRangeMultiplier >= 0)
	{
		NewGrabRange = DefaultGrabRange * ObjectToGrab->CustomGrabRangeMultiplier;
	}

	return NewGrabRange;
}

void UPlayerInteractionBase::InternalSearchForNearestInteractiveObject()
{	
	if(ScpPuGameState->ActiveInteractables.Num() <= 0)
	{
		return;
	}

	const FVector& OwnerLoc =  GetOwner()->GetActorLocation();
	
	UInteractionBase* ClosestIObject = ScpPuGameState->ActiveInteractables[0];
	float ClosestIObjectDistance = FVector::Dist(ClosestIObject->GetOwner()->GetActorLocation(), OwnerLoc);
	
	for(int i = 1; i < ScpPuGameState->ActiveInteractables.Num(); i++)
	{
		const auto& IObj = ScpPuGameState->ActiveInteractables[i];
		const auto& IObjLoc = IObj->GetOwner()->GetActorLocation();
		const auto& IObjDist = FVector::Dist(IObjLoc, OwnerLoc);
		
		if(IObj == CurrentlyGrabbedObject)
		{
			continue;
		}
		
		if(IObjDist < ClosestIObjectDistance)
		{
			ClosestIObject = IObj;
			ClosestIObjectDistance = IObjDist;
		}
	}

	if(ClosestIObjectDistance <= InteractionRange)
	{
		ClosestInteractiveObject = ClosestIObject;
		ClosestInteractiveObjectInfo.bIsValid = true;
		ClosestInteractiveObjectInfo.Class = ClosestInteractiveObject->GetClass();
		ClosestInteractiveObjectInfo.Location = ClosestIObject->GetOwner()->GetActorLocation();

		if(ClosestInteractiveObjectInfo.Class->IsChildOf(UInteractionGrab::StaticClass()))
		{
			const auto& ClosestGrabbableObject = Cast<UInteractionGrab>(ClosestInteractiveObject);

			ClosestInteractiveObjectInfo.PreCalculatedGrabRange = CalculateObjectGrabRange(ClosestGrabbableObject);
		}
		else
		{
			ClosestInteractiveObjectInfo.PreCalculatedGrabRange = -1;
		}
	}
	else
	{
		ClosestInteractiveObject = nullptr;
		ClosestInteractiveObjectInfo = FInteractiveObjectInfo();
	}
}

