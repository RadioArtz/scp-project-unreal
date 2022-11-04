


#include "Attributes/AttributeFloat.h"
#include "Attributes/AttributeFloatModifier.h"

FAttributeFloat::FAttributeFloat()
{
}

void FAttributeFloat::SetBaseValue(float NewValue)
{
	this->BaseValue = NewValue;
	this->ForceModifierRecalculation();
}

float FAttributeFloat::GetFinalValue()
{
	if (!this->bHasBeenInitialized)
	{
		this->ForceModifierRecalculation();
		this->bHasBeenInitialized = true;
	}

	return this->CachedFinalValue;
}

void FAttributeFloat::AddModifier(FName ModifierKey)
{
	this->ModifierKeys.Add(ModifierKey);
	this->ForceModifierRecalculation();
}

bool FAttributeFloat::AddUniqueModifier(FName ModifierKey)
{
	if (this->ModifierKeys.Contains(ModifierKey))
	{
		return false;
	}

	this->ModifierKeys.Add(ModifierKey);
	this->ForceModifierRecalculation();
	return true;
}

void FAttributeFloat::RemoveModifier(FName ModifierKey)
{
	this->ModifierKeys.RemoveSingle(ModifierKey);
	this->ForceModifierRecalculation();
}

int FAttributeFloat::RemoveAllModifiers(FName ModifierKey)
{
	int RemovedElements = this->ModifierKeys.Remove(ModifierKey);
	this->ForceModifierRecalculation();
	return RemovedElements;
}

void FAttributeFloat::ClearModifiers()
{
	this->ModifierKeys.Empty();
	this->ForceModifierRecalculation();
}

void FAttributeFloat::ForceModifierRecalculation()
{
	UDataTable* DataTable = GetDefault<UAttributeFloatModifierSettings>()->ModifierDataTable.LoadSynchronous();
	int AdditionSum = 0;
	int MultiplicationSum = 0;
	int ReductionSum = 0;

	for (auto Elem : this->ModifierKeys)
	{
		FAttributeFloatModifier* Modifier = DataTable->FindRow<FAttributeFloatModifier>(Elem, "");
		if (Modifier == nullptr)
		{
			continue;
		}

		switch (Modifier->Mode)
		{
			case EAttributeFloatModifierMode::Addition:
				AdditionSum = AdditionSum + Modifier->Value;
				break;
			case EAttributeFloatModifierMode::Multiplication:
				MultiplicationSum = MultiplicationSum + Modifier->Value;
				break;
			case EAttributeFloatModifierMode::Reduction:
				ReductionSum = ReductionSum + Modifier->Value;
				break;
			default:
				break;
		}
	}

	this->CachedFinalValue = (this->BaseValue + AdditionSum) * (1 + MultiplicationSum) * (1 - ReductionSum);
}

float UAttributeFloatFunctions::GetBaseValue(FAttributeFloat Attribute)
{
	return Attribute.BaseValue;
}

void UAttributeFloatFunctions::SetBaseValue(UPARAM(ref) FAttributeFloat& Attribute, float NewValue)
{
	Attribute.SetBaseValue(NewValue);
}

float UAttributeFloatFunctions::GetFinalValue(UPARAM(ref) FAttributeFloat& Attribute)
{
	return Attribute.GetFinalValue();
}

void UAttributeFloatFunctions::GetModifierKeys(FAttributeFloat Attribute, TArray<FName>& OutModifierKeys)
{
	OutModifierKeys = Attribute.ModifierKeys;
}

void UAttributeFloatFunctions::AddModifier(UPARAM(ref) FAttributeFloat& Attribute, FName ModifierKey)
{
	return Attribute.AddModifier(ModifierKey);
}

bool UAttributeFloatFunctions::AddUniqueModifier(UPARAM(ref)FAttributeFloat& Attribute, FName ModifierKey)
{
	return Attribute.AddUniqueModifier(ModifierKey);
}

void UAttributeFloatFunctions::RemoveModifier(UPARAM(ref) FAttributeFloat& Attribute, FName ModifierKey)
{
	Attribute.RemoveModifier(ModifierKey);
}

int UAttributeFloatFunctions::RemoveAllModifiers(UPARAM(ref)FAttributeFloat& Attribute, FName ModifierKey)
{
	return Attribute.RemoveAllModifiers(ModifierKey);
}

void UAttributeFloatFunctions::ClearModifiers(UPARAM(ref) FAttributeFloat& Attribute)
{
	Attribute.ClearModifiers();
}

void UAttributeFloatFunctions::ForceModifierRecalculation(UPARAM(ref)FAttributeFloat& Attribute)
{
	Attribute.ForceModifierRecalculation();
}
