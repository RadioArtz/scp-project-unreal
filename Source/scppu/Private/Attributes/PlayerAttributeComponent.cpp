


#include "Attributes/PlayerAttributeComponent.h"
#include "Attributes/FloatAttribute.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UPlayerAttributeComponent::UPlayerAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	this->PrimaryComponentTick.bCanEverTick = true;
	this->SetComponentTickInterval(0.1f);
	this->SetIsReplicatedByDefault(true);

	this->MinValue = FFloatAttribute();
	this->MaxValue = FFloatAttribute();
	this->RegenerationRate = FFloatAttribute();
	this->RegenerationDelay = FFloatAttribute();
}

void UPlayerAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const 
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UPlayerAttributeComponent, CurrentValue);
}

void UPlayerAttributeComponent::SetCurrentValue(float NewValue, bool bSkipRegenerationDelay)
{
	if (!bSkipRegenerationDelay && this->RegenerationDelay.GetFinalValue() > 0)
	{
		FTimerDelegate TimerCallback;
		TimerCallback.BindLambda([this]()
			{
				this->bDelayRegeneration = false;
			});

		this->bDelayRegeneration = true;
		this->GetWorld()->GetTimerManager().ClearTimer(this->RegenerationDelayTimerHandle);
		this->RegenerationDelayTimerHandle.Invalidate();
		this->GetWorld()->GetTimerManager().SetTimer(this->RegenerationDelayTimerHandle, TimerCallback, this->RegenerationDelay.GetFinalValue(), false);
	}

	this->CurrentValue = FMath::Clamp(NewValue, this->MinValue.GetFinalValue(), this->MaxValue.GetFinalValue());

	//make sure to run OnRep locally in case we are the server in a multiplayer scenario.
	if (GetOwner()->HasAuthority()) 
		OnRep_CurrentValue();
}

// Called when the game starts
void UPlayerAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	this->SetCurrentValue(this->CurrentValue, true);
}

// Called every frame
void UPlayerAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!this->bDelayRegeneration)
	{
		this->SetCurrentValue(this->CurrentValue + (this->RegenerationRate.GetFinalValue() * DeltaTime), true);
	}

	BroadcastDelegates();
}

void UPlayerAttributeComponent::BroadcastDelegates() 
{
	if (this->CurrentValue <= this->MinValue.GetFinalValue() && !this->bMinValueReached)
	{
		this->bMinValueReached = true;
		this->OnMinValueReached.Broadcast();
	}
	else if (this->CurrentValue > this->MinValue.GetFinalValue() && this->bMinValueReached)
	{
		this->bMinValueReached = false;
		this->OnMinValueLeft.Broadcast();
	}

	if (this->CurrentValue >= this->MaxValue.GetFinalValue() && !this->bMaxValueReached)
	{
		this->bMaxValueReached = true;
		this->OnMaxValueReached.Broadcast();
	}
	else if (this->CurrentValue < this->MaxValue.GetFinalValue() && this->bMaxValueReached)
	{
		this->bMaxValueReached = false;
		this->OnMaxValueLeft.Broadcast();
	}
}

void UPlayerAttributeComponent::OnRep_CurrentValue()
{
	BroadcastDelegates();
}
