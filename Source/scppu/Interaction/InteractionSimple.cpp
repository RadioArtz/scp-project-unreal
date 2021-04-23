


#include "InteractionSimple.h"


void UInteractionSimple::Interact(UPlayerInteractionBase* Caller)
{
	InteractDelegate.ExecuteIfBound(Caller);
}

bool UInteractionSimple::SetInteractionType(EInteractionType IType)
{
	if(IType < EInteractionType::InteractionGrab)
	{
		return true;
	}

	return false;
}

EInteractionType UInteractionSimple::GetInteractionType_Implementation()
{
	return InteractionType;
}
