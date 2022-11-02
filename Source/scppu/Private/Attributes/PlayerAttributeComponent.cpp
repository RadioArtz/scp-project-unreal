


#include "Attributes/PlayerAttributeComponent.h"
#include "Attributes/AttributeFloat.h"

// Sets default values for this component's properties
UPlayerAttributeComponent::UPlayerAttributeComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	this->PrimaryComponentTick.bCanEverTick = true;
	this->SetComponentTickInterval(0.1f);

	this->MinValue = FAttributeFloat();
	this->MaxValue = FAttributeFloat();
	this->RegenrationRate = FAttributeFloat();
	this->RegenerationDelay = FAttributeFloat();
}

void UPlayerAttributeComponent::SetCurrentValue(float NewValue, bool bSkipRegenrationDelay)
{
	if (!bSkipRegenrationDelay && this->RegenerationDelay.GetFinalValue() > 0)
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
		this->SetCurrentValue(this->CurrentValue + (this->RegenrationRate.GetFinalValue() * DeltaTime), true);
	}

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

