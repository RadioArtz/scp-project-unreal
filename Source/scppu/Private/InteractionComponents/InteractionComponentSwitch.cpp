


#include "InteractionComponents/InteractionComponentSwitch.h"

class AItemBase;

void UInteractionComponentSwitch::SetCurrentState(int NewState)
{
	this->CurrentState = NewState;
	this->MaxState = FMath::Clamp(this->MaxState, 2, 32);
	this->CurrentState = FMath::Clamp(this->CurrentState, 1, this->MaxState);
	this->OnStateChanged.Broadcast((this->CurrentState - 1.0f) / (float)(this->MaxState - 1.0f), this->CurrentState);
}

bool UInteractionComponentSwitch::StartInteraction(APawn* Interactor, AItemBase* Item)
{
	if (!Super::StartInteraction(Interactor, Item))
	{
		return false;
	}

	this->AlphaOffset = (float)(this->CurrentState - 1.0f) / (float)(this->MaxState - 1.0f);
	this->InteractorStartRotation = this->CurrentInteractor->GetControlRotation();
	this->PrimaryComponentTick.SetTickFunctionEnable(true);
	this->OnInteractStart.Broadcast(Interactor, Item);
	return true;
}

bool UInteractionComponentSwitch::EndInteraction(APawn* Interactor)
{
	if (!Super::EndInteraction(Interactor))
	{
		return false;
	}

	this->InteractorStartRotation = FRotator();
	this->PrimaryComponentTick.SetTickFunctionEnable(false);
	this->OnInteractEnd.Broadcast((float)(this->CurrentState - 1.0f) / (float)(this->MaxState - 1.0f), this->CurrentState);
	return true;
}

void UInteractionComponentSwitch::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FRotator Difference = (this->InteractorStartRotation - this->CurrentInteractor->GetControlRotation()).GetNormalized();
	float Alpha = 0.0f;
	switch (this->MovementMode)
	{
	case EInteractionComponentSwitchMoveMode::Horizontal:
		Alpha = FMath::Clamp(Difference.Yaw * this->Sensitivity / 180.0f + AlphaOffset, 0.0f, 1.0f);
		break;
	case EInteractionComponentSwitchMoveMode::Vertical:
		Alpha = FMath::Clamp(Difference.Pitch * this->Sensitivity / 180.0f + AlphaOffset, 0.0f, 1.0f);
		break;
	}

	this->CurrentState = FMath::RoundToInt(FMath::GetMappedRangeValueClamped(FVector2D(0, 1), FVector2D(1, this->MaxState), Alpha));
	this->OnInteractTick.Broadcast(DeltaTime, Alpha, this->CurrentState);
}

void UInteractionComponentSwitch::BeginPlay()
{
	Super::BeginPlay();
	this->SetCurrentState(this->CurrentState);
}
