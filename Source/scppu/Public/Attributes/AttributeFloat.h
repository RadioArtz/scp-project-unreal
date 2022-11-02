

#pragma once

#include "CoreMinimal.h"
#include "AttributeFloat.generated.h"

USTRUCT(BlueprintType)
struct FAttributeFloat
{
	GENERATED_BODY()

	//// Properties ////
public:
	UPROPERTY(EditAnywhere)
		float BaseValue = 0.0f; // get private set

	UPROPERTY(EditAnywhere)
		TArray<FString> Modifiers; // get private set

	//// Functions ////
public:
	FAttributeFloat();

	void SetBaseValue(float NewValue);

	float GetFinalValue();

	void AddModifier(FString Modifier);

	void RemoveModifier(int Index);

	void ClearModifiers(FString Modifier);

};

UCLASS()
class SCPPU_API UAttributeFloatFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

		//// Functions ////	
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		static float GetBaseValue(FAttributeFloat Attribute);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static void SetBaseValue(UPARAM(ref) FAttributeFloat& Attribute, float NewValue);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static float GetFinalValue(FAttributeFloat Attribute);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static void GetModifiers(FAttributeFloat Attribute, TArray<FString>& OutModifiers);

	UFUNCTION(BlueprintCallable)
		static void AddModifier(UPARAM(ref) FAttributeFloat& Attribute, FString Modifier);

	UFUNCTION(BlueprintCallable)
		static void RemoveModifier(UPARAM(ref) FAttributeFloat& Attribute, int Index);

	UFUNCTION(BlueprintCallable)
		static void ClearModifiers(UPARAM(ref) FAttributeFloat& Attribute);
};
