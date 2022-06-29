


#include "InteractionComponent/InteractionComponentButton.h"

void UInteractionComponentButton::StartInteraction(APawn* Interactor, UObject* Item)
{
	Super::StartInteraction(Interactor, Item);
	this->OnInteract.Broadcast(Interactor, Item);
}

void UInteractionComponentButton::StopInteraction(APawn* Interactor)
{
	Super::StopInteraction(Interactor);
}
