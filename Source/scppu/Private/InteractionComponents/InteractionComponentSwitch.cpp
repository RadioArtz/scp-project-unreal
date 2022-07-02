


#include "InteractionComponents/InteractionComponentSwitch.h"

void UInteractionComponentSwitch::StartInteraction(APawn* Interactor, UObject* Item)
{
	Super::StartInteraction(Interactor, Item);
	this->AlphaOffset = (float)(this->CurrentState - 1.0f) / (float)(this->MaxState - 1.0f);
	this->InteractorStartRotation = this->CurrentInteractor->GetControlRotation();
	this->PrimaryComponentTick.SetTickFunctionEnable(true);
	this->OnInteractStart.Broadcast(Interactor, Item);
}

void UInteractionComponentSwitch::EndInteraction(APawn* Interactor)
{
	Super::EndInteraction(Interactor);
	this->InteractorStartRotation = FRotator();
	this->PrimaryComponentTick.SetTickFunctionEnable(false);
	this->OnInteractEnd.Broadcast((float)(this->CurrentState - 1.0f) / (float)(this->MaxState - 1.0f), this->CurrentState);
}

void UInteractionComponentSwitch::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FRotator Difference = (this->InteractorStartRotation - this->CurrentInteractor->GetControlRotation()).GetNormalized();
	UE_LOG(LogTemp, Log, TEXT("%s"), *Difference.ToString());
	float Alpha;
	if (this->MovementMode == EInteractionComponentSwitchMoveMode::Horizontal)
	{
		Alpha = FMath::Clamp(Difference.Yaw * this->Sensitivity / 180.0f + AlphaOffset, 0.0f, 1.0f);
	}
	else if (this->MovementMode == EInteractionComponentSwitchMoveMode::Vertical)
	{
		Alpha = FMath::Clamp(Difference.Pitch * this->Sensitivity / 180.0f + AlphaOffset, 0.0f, 1.0f);
	}

	this->CurrentState = FMath::RoundToInt(FMath::GetMappedRangeValueClamped(FVector2D(0, 1), FVector2D(1, this->MaxState), Alpha));
	this->OnInteractTick.Broadcast(DeltaTime, Alpha, this->CurrentState);
}

void UInteractionComponentSwitch::BeginPlay()
{
	Super::BeginPlay();
	this->MaxState = FMath::Clamp(this->MaxState, 2, 10);
	this->CurrentState = FMath::Clamp(this->CurrentState, 1, this->MaxState);
	this->OnInteractEnd.Broadcast((this->CurrentState - 1.0f) / (float)(this->MaxState - 1.0f), this->CurrentState);
}
