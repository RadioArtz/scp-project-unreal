


#include "Attributes/AttributeFloat.h"

FAttributeFloat::FAttributeFloat()
{
}

void FAttributeFloat::SetBaseValue(float NewValue)
{
	this->BaseValue = NewValue;
}

float FAttributeFloat::GetFinalValue()
{
	return BaseValue;
}

void FAttributeFloat::AddModifier(FString Modifier)
{
}

void FAttributeFloat::RemoveModifier(int Index)
{
}

void FAttributeFloat::ClearModifiers(FString Modifier)
{
}

float UAttributeFloatFunctions::GetBaseValue(FAttributeFloat Attribute)
{
	return Attribute.BaseValue;
}

void UAttributeFloatFunctions::SetBaseValue(UPARAM(ref) FAttributeFloat& Attribute, float NewValue)
{
	Attribute.SetBaseValue(NewValue);
}

float UAttributeFloatFunctions::GetFinalValue(FAttributeFloat Attribute)
{
	return Attribute.GetFinalValue();
}

void UAttributeFloatFunctions::GetModifiers(FAttributeFloat Attribute, TArray<FString>& OutModifiers)
{
	OutModifiers = Attribute.Modifiers;
}

void UAttributeFloatFunctions::AddModifier(UPARAM(ref) FAttributeFloat& Attribute, FString Modifier)
{
}

void UAttributeFloatFunctions::RemoveModifier(UPARAM(ref) FAttributeFloat& Attribute, int Index)
{
}

void UAttributeFloatFunctions::ClearModifiers(UPARAM(ref) FAttributeFloat& Attribute)
{
}
