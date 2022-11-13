


#include "InteractionComponents/ButtonInteractionComponent.h"

class ABaseItem;

bool UButtonInteractionComponent::BeginInteraction(APawn* Interactor, ABaseItem* Item)
{
	if (!Super::BeginInteraction(Interactor, Item))
	{
		return false;
	}

	this->OnInteract.Broadcast(Interactor, Item);
	return true;
}

bool UButtonInteractionComponent::EndInteraction(APawn* Interactor)
{
	return Super::EndInteraction(Interactor);
}
