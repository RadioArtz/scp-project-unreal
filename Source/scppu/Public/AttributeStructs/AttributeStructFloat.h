

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AttributeStructFloat.generated.h"

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_DELEGATE(FAttributeStructFloatValueReached);

USTRUCT(BlueprintType)
struct FAttributeStructFloat
{
	GENERATED_BODY()

	//// Properties ////
private:
	// Min value that the returned value can have.
	UPROPERTY(EditAnywhere)
		float MinValue = 0.0f;

	// Max value that the returned value can have.
	UPROPERTY(EditAnywhere)
		float MaxValue = 100.0f;

	// Regeneration rate of the returned value in n/sec.
	UPROPERTY(EditAnywhere)
		float RegenerationRate = 1.0f;

	// How long to wait in seconds before regenerating the returned value after it got changed via 'SetCurrentValue'.
	UPROPERTY(EditAnywhere)
		float RegenerationDelay = 0.0f;

	UPROPERTY(EditAnywhere, meta=(DisplayName = "Start Value"))
		float LastSavedValue;

	UPROPERTY()
		float LastSavedValueGameTime = 0.0f;

	UPROPERTY()
		FTimerHandle OnMinValueReachedHandle;

	UPROPERTY()
		bool bOnMinValueReachedExecuted = false;

	UPROPERTY()
		FTimerHandle OnMaxValueReachedHandle;

	UPROPERTY()
		bool bOnMaxValueReachedExecuted = false;

	//// Functions ////	
public:
	void ClearOnMinValueReached(UObject* WorldContextObject);

	void BindOnMinValueReached(UObject* WorldContextObject, FAttributeStructFloatValueReached OnMinValueReached);

	void ClearOnMaxValueReached(UObject* WorldContextObject);

	void BindOnMaxValueReached(UObject* WorldContextObject, FAttributeStructFloatValueReached OnMaxValueReached);

	float GetMinValue();

	void SetMinValue(UObject* WorldContextObject, float Value);

	float GetMaxValue();

	void SetMaxValue(UObject* WorldContextObject, float Value);

	float GetRegenerationRate();

	void SetRegenerationRate(UObject* WorldContextObject, float Value);

	float GetRegenerationDelay();

	void SetRegenerationDelay(UObject* WorldContextObject, float Value);

	float GetCurrentValue(UObject* WorldContextObject);

	void SetCurrentValue(UObject* WorldContextObject, float Value, bool bSkipRegenerationDelay = false);
};

UCLASS()
class SCPPU_API UFloatAttributeFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	//// Functions ////	
public:
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", DisplayName = "Clear On Min Value Reached"))
		static void ClearOnMinValueReached(UObject* WorldContextObject, UPARAM(ref) FAttributeStructFloat& FloatAttribute);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", DisplayName = "Bind On Min Value Reached"))
		static void BindOnMinValueReached(UObject* WorldContextObject, UPARAM(ref) FAttributeStructFloat& FloatAttribute, FAttributeStructFloatValueReached OnMinValueReached);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", DisplayName = "Clear On Max Value Reached"))
		static void ClearOnMaxValueReached(UObject* WorldContextObject, UPARAM(ref) FAttributeStructFloat& FloatAttribute);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject", DisplayName = "Bind On Max Value Reached"))
		static void BindOnMaxValueReached(UObject* WorldContextObject, UPARAM(ref) FAttributeStructFloat& FloatAttribute, FAttributeStructFloatValueReached OnMaxValueReached);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static float GetMinValue(FAttributeStructFloat FloatAttribute);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static void SetMinValue(UObject* WorldContextObject, UPARAM(ref) FAttributeStructFloat& FloatAttribute, float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static float GetMaxValue(FAttributeStructFloat FloatAttribute);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static void SetMaxValue(UObject* WorldContextObject, UPARAM(ref) FAttributeStructFloat& FloatAttribute, float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static float GetRegenerationRate(FAttributeStructFloat FloatAttribute);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static void SetRegenerationRate(UObject* WorldContextObject, UPARAM(ref) FAttributeStructFloat& FloatAttribute, float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static float GetRegenerationDelay(FAttributeStructFloat FloatAttribute);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static void SetRegenerationDelay(UObject* WorldContextObject, UPARAM(ref) FAttributeStructFloat& FloatAttribute, float Value);

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
		static float GetCurrentValue(UObject* WorldContextObject, FAttributeStructFloat FloatAttribute);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static void SetCurrentValue(UObject* WorldContextObject, UPARAM(ref) FAttributeStructFloat& FloatAttribute, float Value, bool bSkipRegenerationDelay);
};
