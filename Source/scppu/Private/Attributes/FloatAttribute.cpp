


#include "Attributes/FloatAttribute.h"
#include "Attributes/FloatAttributeModifier.h"

FFloatAttribute::FFloatAttribute()
{
}

void FFloatAttribute::SetBaseValue(float NewValue)
{
	this->BaseValue = NewValue;
	this->ForceModifierRecalculation();
}

float FFloatAttribute::GetFinalValue()
{
	if (!this->bHasBeenInitialized)
	{
		this->ForceModifierRecalculation();
		this->bHasBeenInitialized = true;
	}

	return this->CachedFinalValue;
}

void FFloatAttribute::AddModifier(FName ModifierKey)
{
	this->ModifierKeys.Add(ModifierKey);
	this->ForceModifierRecalculation();
}

bool FFloatAttribute::AddUniqueModifier(FName ModifierKey)
{
	if (this->ModifierKeys.Contains(ModifierKey))
	{
		return false;
	}

	this->ModifierKeys.Add(ModifierKey);
	this->ForceModifierRecalculation();
	return true;
}

void FFloatAttribute::RemoveModifier(FName ModifierKey)
{
	this->ModifierKeys.RemoveSingle(ModifierKey);
	this->ForceModifierRecalculation();
}

int FFloatAttribute::RemoveAllModifiers(FName ModifierKey)
{
	int RemovedElements = this->ModifierKeys.Remove(ModifierKey);
	this->ForceModifierRecalculation();
	return RemovedElements;
}

void FFloatAttribute::ClearModifiers()
{
	this->ModifierKeys.Empty();
	this->ForceModifierRecalculation();
}

void FFloatAttribute::ForceModifierRecalculation()
{
	UDataTable* DataTable = GetDefault<UFloatAttributeModifierSettings>()->AttributeModifierDataTable.LoadSynchronous();
	int AdditionSum = 0;
	int MultiplicationSum = 0;
	int ReductionSum = 0;

	for (auto Elem : this->ModifierKeys)
	{
		FFloatAttributeModifier* Modifier = DataTable->FindRow<FFloatAttributeModifier>(Elem, "");
		if (Modifier == nullptr)
		{
			continue;
		}

		switch (Modifier->Mode)
		{
			case EFloatAttributeModifierMode::Addition:
				AdditionSum = AdditionSum + Modifier->Value;
				break;
			case EFloatAttributeModifierMode::Multiplication:
				MultiplicationSum = MultiplicationSum + Modifier->Value;
				break;
			case EFloatAttributeModifierMode::Reduction:
				ReductionSum = ReductionSum + Modifier->Value;
				break;
			default:
				break;
		}
	}

	this->CachedFinalValue = (this->BaseValue + AdditionSum) * (1 + MultiplicationSum) * (1 - ReductionSum);
}

float UAttributeFloatFunctions::GetBaseValue(FFloatAttribute Attribute)
{
	return Attribute.BaseValue;
}

void UAttributeFloatFunctions::SetBaseValue(UPARAM(ref) FFloatAttribute& Attribute, float NewValue)
{
	Attribute.SetBaseValue(NewValue);
}

float UAttributeFloatFunctions::GetFinalValue(UPARAM(ref) FFloatAttribute& Attribute)
{
	return Attribute.GetFinalValue();
}

void UAttributeFloatFunctions::GetModifierKeys(FFloatAttribute Attribute, TArray<FName>& OutModifierKeys)
{
	OutModifierKeys = Attribute.ModifierKeys;
}

void UAttributeFloatFunctions::AddModifier(UPARAM(ref) FFloatAttribute& Attribute, FName ModifierKey)
{
	return Attribute.AddModifier(ModifierKey);
}

bool UAttributeFloatFunctions::AddUniqueModifier(UPARAM(ref)FFloatAttribute& Attribute, FName ModifierKey)
{
	return Attribute.AddUniqueModifier(ModifierKey);
}

void UAttributeFloatFunctions::RemoveModifier(UPARAM(ref) FFloatAttribute& Attribute, FName ModifierKey)
{
	Attribute.RemoveModifier(ModifierKey);
}

int UAttributeFloatFunctions::RemoveAllModifiers(UPARAM(ref)FFloatAttribute& Attribute, FName ModifierKey)
{
	return Attribute.RemoveAllModifiers(ModifierKey);
}

void UAttributeFloatFunctions::ClearModifiers(UPARAM(ref) FFloatAttribute& Attribute)
{
	Attribute.ClearModifiers();
}

void UAttributeFloatFunctions::ForceModifierRecalculation(UPARAM(ref)FFloatAttribute& Attribute)
{
	Attribute.ForceModifierRecalculation();
}
