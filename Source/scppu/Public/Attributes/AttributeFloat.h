

#pragma once

#include "CoreMinimal.h"
#include "AttributeFloat.generated.h"

class UAttributeFloatModifierBase;

USTRUCT(BlueprintType)
struct FAttributeFloat
{
	GENERATED_BODY()

	//// Properties ////
public:
	UPROPERTY(EditAnywhere)
		float BaseValue = 0.0f; // get private set

	UPROPERTY(EditAnywhere)
		TArray<TSubclassOf<UAttributeFloatModifierBase>> Modifiers; // get private set

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

	int AddModifier(TSubclassOf<UAttributeFloatModifierBase> Modifier);

	void RemoveModifier(int Index);

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
		static float GetFinalValue(FAttributeFloat Attribute);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static void GetModifiers(FAttributeFloat Attribute, TArray<TSubclassOf<UAttributeFloatModifierBase>>& OutModifiers);

	UFUNCTION(BlueprintCallable)
		static int AddModifier(UPARAM(ref) FAttributeFloat& Attribute, TSubclassOf<UAttributeFloatModifierBase> Modifier);

	UFUNCTION(BlueprintCallable)
		static void RemoveModifier(UPARAM(ref) FAttributeFloat& Attribute, int Index);

	UFUNCTION(BlueprintCallable)
		static void ClearModifiers(UPARAM(ref) FAttributeFloat& Attribute);

	UFUNCTION(BlueprintCallable)
		static void ForceModifierRecalculation(UPARAM(ref) FAttributeFloat& Attribute);
};
