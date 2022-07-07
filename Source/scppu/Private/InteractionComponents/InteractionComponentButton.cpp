


#include "InteractionComponents/InteractionComponentButton.h"

class AItemBase;

bool UInteractionComponentButton::StartInteraction(APawn* Interactor, AItemBase* Item)
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
