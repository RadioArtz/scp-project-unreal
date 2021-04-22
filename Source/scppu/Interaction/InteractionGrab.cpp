


#include "InteractionGrab.h"

void UInteractionGrab::InteractGrab_Implementation(UPlayerInteractionBase* Caller)
{
	InteractGrabDelegate.ExecuteIfBound(Caller);

	HoldingPlayer = Caller;
}

void UInteractionGrab::InteractDrop_Implementation(UPlayerInteractionBase* Caller)
{
	InteractDropDelegate.ExecuteIfBound(Caller);

	HoldingPlayer = nullptr;
}

EInteractionType UInteractionGrab::GetInteractionType_Implementation()
{
	if(HoldingPlayer)
	{
		return EInteractionType::NoInteraction;
	}
	
	return EInteractionType::InteractionGrab;
}
