

#pragma once

#include "CoreMinimal.h"
#include "FloatAttribute.generated.h"

USTRUCT(BlueprintType)
struct FFloatAttribute
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
	FFloatAttribute();

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
		static float GetBaseValue(FFloatAttribute Attribute);

	UFUNCTION(BlueprintCallable)
		static void SetBaseValue(UPARAM(ref) FFloatAttribute& Attribute, float NewValue);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static float GetFinalValue(UPARAM(ref) FFloatAttribute& Attribute);

	UFUNCTION(BlueprintCallable)
		static void GetModifierKeys(FFloatAttribute Attribute, TArray<FName>& OutModifierKeys);

	UFUNCTION(BlueprintCallable)
		static void AddModifier(UPARAM(ref) FFloatAttribute& Attribute, FName ModifierKey);

	UFUNCTION(BlueprintCallable)
		static bool AddUniqueModifier(UPARAM(ref) FFloatAttribute& Attribute, FName ModifierKey);

	UFUNCTION(BlueprintCallable)
		static void RemoveModifier(UPARAM(ref) FFloatAttribute& Attribute, FName ModifierKey);

	UFUNCTION(BlueprintCallable)
		static int RemoveAllModifiers(UPARAM(ref) FFloatAttribute& Attribute, FName ModifierKey);

	UFUNCTION(BlueprintCallable)
		static void ClearModifiers(UPARAM(ref) FFloatAttribute& Attribute);

	UFUNCTION(BlueprintCallable)
		static void ForceModifierRecalculation(UPARAM(ref) FFloatAttribute& Attribute);
};
