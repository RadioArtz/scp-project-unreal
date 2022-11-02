


#include "Attributes/AttributeFloat.h"
#include "Attributes/AttributeFloatModifierBase.h"

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

int FAttributeFloat::AddModifier(TSubclassOf<UAttributeFloatModifierBase> Modifier)
{
	this->Modifiers.Add(Modifier);
	this->ForceModifierRecalculation();
	return this->Modifiers.Num() - 1;
}

void FAttributeFloat::RemoveModifier(int Index)
{
	this->Modifiers.RemoveAt(Index);
	this->ForceModifierRecalculation();
}

void FAttributeFloat::ClearModifiers()
{
	this->Modifiers.Empty();
	this->ForceModifierRecalculation();
}

void FAttributeFloat::ForceModifierRecalculation()
{
	this->CachedFinalValue = this->BaseValue;
	for (auto Elem : this->Modifiers)
	{
		if (!IsValid(Elem))
		{
			continue;
		}

		this->CachedFinalValue = this->CachedFinalValue + Elem.GetDefaultObject()->GetModifiedValue(this->BaseValue);
	}
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

void UAttributeFloatFunctions::GetModifiers(FAttributeFloat Attribute, TArray<TSubclassOf<UAttributeFloatModifierBase>>& OutModifiers)
{
	OutModifiers = Attribute.Modifiers;
}

int UAttributeFloatFunctions::AddModifier(UPARAM(ref) FAttributeFloat& Attribute, TSubclassOf<UAttributeFloatModifierBase> Modifier)
{
	return Attribute.AddModifier(Modifier);
}

void UAttributeFloatFunctions::RemoveModifier(UPARAM(ref) FAttributeFloat& Attribute, int Index)
{
	Attribute.RemoveModifier(Index);
}

void UAttributeFloatFunctions::ClearModifiers(UPARAM(ref) FAttributeFloat& Attribute)
{
	Attribute.ClearModifiers();
}

void UAttributeFloatFunctions::ForceModifierRecalculation(UPARAM(ref)FAttributeFloat& Attribute)
{
	Attribute.ForceModifierRecalculation();
}
