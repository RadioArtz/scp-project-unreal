


#include "InteractionComponents/InteractionComponentButton.h"

bool UInteractionComponentButton::StartInteraction(APawn* Interactor, UObject* Item)
{
	if (!Super::StartInteraction(Interactor, Item))
	{
		return false;
	}

	this->OnInteract.Broadcast(Interactor, Item);
	return true;
}

bool UInteractionComponentButton::EndInteraction(APawn* Interactor)
{
	return Super::EndInteraction(Interactor);
}
