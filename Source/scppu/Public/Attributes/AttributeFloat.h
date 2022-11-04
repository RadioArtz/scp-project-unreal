

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
		TArray<FName> ModifierKeys; // get private set

private:
	UPROPERTY()
		float CachedFinalValue = 0.0f;

	UPROPERTY()
		bool bHasBeenInitialized = false;

	//// Functions ////
public:
	FAttributeFloat();

	void SetBaseValue(float NewValue);

	float GetFinalValue();

	void AddModifier(FName ModifierKey);

	bool AddUniqueModifier(FName ModifierKey);

	void RemoveModifier(FName ModifierKey);

	int RemoveAllModifiers(FName ModifierKey);

	void ClearModifiers();

	void ForceModifierRecalculation();

};

UCLASS()
class SCPPU_API UAttributeFloatFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	//// Functions ////	
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		static float GetBaseValue(FAttributeFloat Attribute);

	UFUNCTION(BlueprintCallable)
		static void SetBaseValue(UPARAM(ref) FAttributeFloat& Attribute, float NewValue);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static float GetFinalValue(UPARAM(ref) FAttributeFloat& Attribute);

	UFUNCTION(BlueprintCallable)
		static void GetModifierKeys(FAttributeFloat Attribute, TArray<FName>& OutModifierKeys);

	UFUNCTION(BlueprintCallable)
		static void AddModifier(UPARAM(ref) FAttributeFloat& Attribute, FName ModifierKey);

	UFUNCTION(BlueprintCallable)
		static bool AddUniqueModifier(UPARAM(ref) FAttributeFloat& Attribute, FName ModifierKey);

	UFUNCTION(BlueprintCallable)
		static void RemoveModifier(UPARAM(ref) FAttributeFloat& Attribute, FName ModifierKey);

	UFUNCTION(BlueprintCallable)
		static int RemoveAllModifiers(UPARAM(ref) FAttributeFloat& Attribute, FName ModifierKey);

	UFUNCTION(BlueprintCallable)
		static void ClearModifiers(UPARAM(ref) FAttributeFloat& Attribute);

	UFUNCTION(BlueprintCallable)
		static void ForceModifierRecalculation(UPARAM(ref) FAttributeFloat& Attribute);
};
