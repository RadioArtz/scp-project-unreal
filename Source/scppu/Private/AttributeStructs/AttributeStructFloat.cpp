


#include "AttributeStructs/AttributeStructFloat.h"

void FAttributeStructFloat::ClearOnMinValueReached(UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject))
	{
		return;
	}

	WorldContextObject->GetWorld()->GetTimerManager().ClearTimer(this->OnMinValueReachedHandle);
}

void FAttributeStructFloat::SetOnMinValueReached(UObject* WorldContextObject, FAttributeStructFloatValueReached Event)
{
	if (!IsValid(WorldContextObject))
	{
		return;
	}

	this->ClearOnMinValueReached(WorldContextObject);
	WorldContextObject->GetWorld()->GetTimerManager().SetTimer(this->OnMinValueReachedHandle, [this, Event, WorldContextObject]()
	{
		if (this->GetCurrentValue(WorldContextObject) <= this->MinValue && !this->bOnMinValueReachedExecuted)
		{
			Event.ExecuteIfBound();
			this->bOnMinValueReachedExecuted = true;
		}
		else if (this->GetCurrentValue(WorldContextObject) > this->MinValue)
		{
			this->bOnMinValueReachedExecuted = false;
		}
	}, 0.1f, true);
}

void FAttributeStructFloat::ClearOnMaxValueReached(UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject))
	{
		return;
	}

	WorldContextObject->GetWorld()->GetTimerManager().ClearTimer(this->OnMaxValueReachedHandle);
}

void FAttributeStructFloat::SetOnMaxValueReached(UObject* WorldContextObject, FAttributeStructFloatValueReached Event)
{
	if (!IsValid(WorldContextObject))
	{
		return;
	}

	this->ClearOnMaxValueReached(WorldContextObject);
	WorldContextObject->GetWorld()->GetTimerManager().SetTimer(this->OnMaxValueReachedHandle, [this, Event, WorldContextObject]()
	{
		if (this->GetCurrentValue(WorldContextObject) >= this->MaxValue && !this->bOnMaxValueReachedExecuted)
		{
			Event.ExecuteIfBound();
			this->bOnMaxValueReachedExecuted = true;
		}
		else if (this->GetCurrentValue(WorldContextObject) < this->MaxValue)
		{
			this->bOnMaxValueReachedExecuted = false;
		}
	}, 0.1f, true);
}

float FAttributeStructFloat::GetMinValue()
{
	return this->MinValue;
}

void FAttributeStructFloat::SetMinValue(UObject* WorldContextObject, float Value)
{
	if (!IsValid(WorldContextObject))
	{
		return;
	}

	this->SetCurrentValue(WorldContextObject, this->GetCurrentValue(WorldContextObject), true);
	this->MinValue = Value;
}

float FAttributeStructFloat::GetMaxValue()
{
	return this->MaxValue;
}

void FAttributeStructFloat::SetMaxValue(UObject* WorldContextObject, float Value)
{
	if (!IsValid(WorldContextObject))
	{
		return;
	}

	this->SetCurrentValue(WorldContextObject, this->GetCurrentValue(WorldContextObject), true);
	this->MaxValue = Value;
}

float FAttributeStructFloat::GetRegenerationRate()
{
	return this->RegenerationRate;
}

void FAttributeStructFloat::SetRegenerationRate(UObject* WorldContextObject, float Value)
{
	if (!IsValid(WorldContextObject))
	{
		return;
	}

	this->SetCurrentValue(WorldContextObject, this->GetCurrentValue(WorldContextObject), true);
	this->RegenerationRate = Value;
}

float FAttributeStructFloat::GetRegenerationDelay()
{
	return this->RegenerationDelay;
}

void FAttributeStructFloat::SetRegenerationDelay(UObject* WorldContextObject, float Value)
{
	if (!IsValid(WorldContextObject))
	{
		return;
	}

	this->SetCurrentValue(WorldContextObject, this->GetCurrentValue(WorldContextObject), true);
	this->RegenerationDelay = Value;
}

float FAttributeStructFloat::GetCurrentValue(UObject* WorldContextObject)
{
	if (!IsValid(WorldContextObject))
	{
		return 0.0f;
	}

	return FMath::Clamp(this->LastSavedValue + (this->RegenerationRate * FMath::Clamp(WorldContextObject->GetWorld()->TimeSeconds - this->LastSavedValueGameTime - this->RegenerationDelay, 0.0f, 1e18f)), this->MinValue, this->MaxValue);
}

void FAttributeStructFloat::SetCurrentValue(UObject* WorldContextObject, float Value, bool bSkipRegenerationDelay)
{
	if (!IsValid(WorldContextObject))
	{
		return;
	}

	this->LastSavedValue = Value;
	if (bSkipRegenerationDelay)
	{
		this->LastSavedValueGameTime = WorldContextObject->GetWorld()->TimeSeconds - this->RegenerationDelay;
	}
	else
	{
		this->LastSavedValueGameTime = WorldContextObject->GetWorld()->TimeSeconds;
	}
}

void UFloatAttributeFunctions::ClearOnMinValueReached(UObject* WorldContextObject, UPARAM(ref)FAttributeStructFloat& FloatAttribute)
{
	return FloatAttribute.ClearOnMinValueReached(WorldContextObject);
}

void UFloatAttributeFunctions::SetOnMinValueReached(UObject* WorldContextObject, UPARAM(ref)FAttributeStructFloat& FloatAttribute, FAttributeStructFloatValueReached Event)
{
	return FloatAttribute.SetOnMinValueReached(WorldContextObject, Event);
}

void UFloatAttributeFunctions::ClearOnMaxValueReached(UObject* WorldContextObject, UPARAM(ref)FAttributeStructFloat& FloatAttribute)
{
	return FloatAttribute.ClearOnMaxValueReached(WorldContextObject);
}

void UFloatAttributeFunctions::SetOnMaxValueReached(UObject* WorldContextObject, UPARAM(ref)FAttributeStructFloat& FloatAttribute, FAttributeStructFloatValueReached Event)
{
	return FloatAttribute.SetOnMaxValueReached(WorldContextObject, Event);
}

float UFloatAttributeFunctions::GetMinValue(FAttributeStructFloat FloatAttribute)
{
	return FloatAttribute.GetMinValue();
}

void UFloatAttributeFunctions::SetMinValue(UObject* WorldContextObject, UPARAM(ref)FAttributeStructFloat& FloatAttribute, float Value)
{
	return FloatAttribute.SetMinValue(WorldContextObject, Value);
}

float UFloatAttributeFunctions::GetMaxValue(FAttributeStructFloat FloatAttribute)
{
	return FloatAttribute.GetMaxValue();
}

void UFloatAttributeFunctions::SetMaxValue(UObject* WorldContextObject, UPARAM(ref)FAttributeStructFloat& FloatAttribute, float Value)
{
	return FloatAttribute.SetMaxValue(WorldContextObject, Value);
}

float UFloatAttributeFunctions::GetRegenerationRate(FAttributeStructFloat FloatAttribute)
{
	return FloatAttribute.GetRegenerationRate();
}

void UFloatAttributeFunctions::SetRegenerationRate(UObject* WorldContextObject, UPARAM(ref)FAttributeStructFloat& FloatAttribute, float Value)
{
	return FloatAttribute.SetRegenerationRate(WorldContextObject, Value);
}

float UFloatAttributeFunctions::GetRegenerationDelay(FAttributeStructFloat FloatAttribute)
{
	return FloatAttribute.GetRegenerationDelay();
}

void UFloatAttributeFunctions::SetRegenerationDelay(UObject* WorldContextObject, UPARAM(ref)FAttributeStructFloat& FloatAttribute, float Value)
{
	return FloatAttribute.SetRegenerationDelay(WorldContextObject, Value);
}

float UFloatAttributeFunctions::GetCurrentValue(UObject* WorldContextObject, FAttributeStructFloat FloatAttribute)
{
	return FloatAttribute.GetCurrentValue(WorldContextObject);
}

void UFloatAttributeFunctions::SetCurrentValue(UObject* WorldContextObject, UPARAM(ref)FAttributeStructFloat& FloatAttribute, float Value, bool bSkipRegenerationDelay)
{
	return FloatAttribute.SetCurrentValue(WorldContextObject, Value, bSkipRegenerationDelay);
}
