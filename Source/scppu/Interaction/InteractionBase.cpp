


#include "InteractionBase.h"


#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "scppu/GameLogic/ScpPuGameStateBase.h"

UInteractionBase::UInteractionBase()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UInteractionBase::BeginPlay()
{
	Super::BeginPlay();

	const auto GameState = GetWorld()->GetGameState<AScpPuGameStateBase>();

	if(GameState)
	{
		GameState->ActiveInteractables.Push(this);
	}
	
}

EInteractionType UInteractionBase::GetInteractionType_Implementation()
{
	return EInteractionType::NoInteraction;
}

UTexture2D* UInteractionBase::GetInteractionImage()
{
	return nullptr;
}

