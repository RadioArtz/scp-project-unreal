

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FloatAttribute.generated.h"

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_DELEGATE(FFloatAttributeValueReached);

USTRUCT(BlueprintType)
struct FFloatAttribute
{
	GENERATED_BODY()

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
		float LastChangedValue;

	UPROPERTY()
		float LastChangedValueGameTime = 0.0f;

	UPROPERTY()
		FFloatAttributeValueReached OnMinValueReachedDel;

	UPROPERTY()
		FTimerHandle OnMinValueReachedHandle;

	UPROPERTY()
		FFloatAttributeValueReached OnMaxValueReachedDel;

	UPROPERTY()
		FTimerHandle OnMaxValueReachedHandle;

public:
	void ClearOnMinValueReached()
	{
		this->OnMinValueReachedDel.Unbind();
		this->OnMinValueReachedHandle.Invalidate();
	}

	void BindOnMinValueReached(UObject* WorldContextObject, FFloatAttributeValueReached OnMinValueReached)
	{
		float TimerDelay = 0.0f;
		this->ClearOnMinValueReached();
		this->OnMinValueReachedDel = OnMinValueReached;
		WorldContextObject->GetWorld()->GetTimerManager().SetTimer(this->OnMinValueReachedHandle, FTimerDynamicDelegate(OnMinValueReached), TimerDelay, false);
	}

	void ClearOnMaxValueReached()
	{
		this->OnMaxValueReachedDel.Unbind();
		this->OnMaxValueReachedHandle.Invalidate();
	}

	void BindOnMaxValueReached(UObject* WorldContextObject, FFloatAttributeValueReached OnMaxValueReached)
	{
		float TimerDelay = 0.0f;
		this->ClearOnMaxValueReached();
		this->OnMaxValueReachedDel = OnMaxValueReached;
		WorldContextObject->GetWorld()->GetTimerManager().SetTimer(this->OnMaxValueReachedHandle, FTimerDynamicDelegate(OnMaxValueReached), TimerDelay, false);
	}

	float GetMinValue()
	{
		return this->MinValue;
	}

	void SetMinValue(UObject* WorldContextObject, float Value)
	{
		this->SetCurrentValue(WorldContextObject, this->GetCurrentValue(WorldContextObject), true);
		this->MinValue = Value;
	}

	float GetMaxValue()
	{
		return this->MaxValue;
	}

	void SetMaxValue(UObject* WorldContextObject, float Value)
	{
		this->SetCurrentValue(WorldContextObject, this->GetCurrentValue(WorldContextObject), true);
		this->MaxValue = Value;
	}

	float GetRegenerationRate()
	{
		return this->RegenerationRate;
	}

	void SetRegenerationRate(UObject* WorldContextObject, float Value)
	{
		this->SetCurrentValue(WorldContextObject, this->GetCurrentValue(WorldContextObject), true);
		this->RegenerationRate = Value;
	}

	float GetRegenerationDelay()
	{
		return this->RegenerationDelay;
	}

	void SetRegenerationDelay(UObject* WorldContextObject, float Value)
	{
		this->SetCurrentValue(WorldContextObject, this->GetCurrentValue(WorldContextObject), true);
		this->RegenerationDelay = Value;
	}

	float GetCurrentValue(UObject* WorldContextObject)
	{
		if (!IsValid(WorldContextObject->GetWorld()))
		{
			return 0.0f;
		}

		return FMath::Clamp(this->LastChangedValue + (this->RegenerationRate * FMath::Clamp(WorldContextObject->GetWorld()->TimeSeconds - this->LastChangedValueGameTime - this->RegenerationDelay, 0.0f, 1e18f)), this->MinValue, this->MaxValue);
	}

	void SetCurrentValue(UObject* WorldContextObject, float Value, bool bSkipRegenerationDelay = false)
	{
		if (!IsValid(WorldContextObject->GetWorld()))
		{
			return;
		}

		this->LastChangedValue = Value;
		if (bSkipRegenerationDelay)
		{
			this->LastChangedValueGameTime = WorldContextObject->GetWorld()->TimeSeconds - this->RegenerationDelay;
		}
		else
		{
			this->LastChangedValueGameTime = WorldContextObject->GetWorld()->TimeSeconds;
		}

		this->BindOnMinValueReached(WorldContextObject, this->OnMinValueReachedDel);
	}
};

UCLASS()
class SCPPU_API UFloatAttributeFunctions : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static void ClearOnMinValueReached(UPARAM(ref) FFloatAttribute& FloatAttribute) { return FloatAttribute.ClearOnMinValueReached(); }

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static void BindOnMinValueReached(UObject* WorldContextObject, UPARAM(ref) FFloatAttribute& FloatAttribute, FFloatAttributeValueReached OnMinValueReached) { return FloatAttribute.BindOnMinValueReached(WorldContextObject, OnMinValueReached); }

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static void ClearOnMaxValueReached(UPARAM(ref) FFloatAttribute& FloatAttribute) { return FloatAttribute.ClearOnMaxValueReached(); }

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static void BindOnMaxValueReached(UObject* WorldContextObject, UPARAM(ref) FFloatAttribute& FloatAttribute, FFloatAttributeValueReached OnMaxValueReached) { return FloatAttribute.BindOnMaxValueReached(WorldContextObject, OnMaxValueReached); }

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
		static float GetMinValue(UPARAM(ref) FFloatAttribute& FloatAttribute) { return FloatAttribute.GetMinValue(); }

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static void SetMinValue(UObject* WorldContextObject, UPARAM(ref) FFloatAttribute& FloatAttribute, float Value) { return FloatAttribute.SetMinValue(WorldContextObject, Value); }

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
		static float GetMaxValue(UPARAM(ref) FFloatAttribute& FloatAttribute) { return FloatAttribute.GetMaxValue(); }

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static void SetMaxValue(UObject* WorldContextObject, UPARAM(ref) FFloatAttribute& FloatAttribute, float Value) { return FloatAttribute.SetMaxValue(WorldContextObject, Value); }

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
		static float GetRegenerationRate(UPARAM(ref) FFloatAttribute& FloatAttribute) { return FloatAttribute.GetRegenerationRate(); }

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static void SetRegenerationRate(UObject* WorldContextObject, UPARAM(ref) FFloatAttribute& FloatAttribute, float Value) { return FloatAttribute.SetRegenerationRate(WorldContextObject, Value); }

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
		static float GetRegenerationDelay(UPARAM(ref) FFloatAttribute& FloatAttribute) { return FloatAttribute.GetRegenerationDelay(); }

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static void SetRegenerationDelay(UObject* WorldContextObject, UPARAM(ref) FFloatAttribute& FloatAttribute, float Value) { return FloatAttribute.SetRegenerationDelay(WorldContextObject, Value); }

	UFUNCTION(BlueprintCallable, BlueprintPure, meta = (WorldContext = "WorldContextObject"))
		static float GetCurrentValue(UObject* WorldContextObject, UPARAM(ref) FFloatAttribute& FloatAttribute) { return FloatAttribute.GetCurrentValue(WorldContextObject); }

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static void SetCurrentValue(UObject* WorldContextObject, UPARAM(ref) FFloatAttribute& FloatAttribute, float Value, bool bSkipRegenerationDelay) { return FloatAttribute.SetCurrentValue(WorldContextObject, Value, bSkipRegenerationDelay); }
};
