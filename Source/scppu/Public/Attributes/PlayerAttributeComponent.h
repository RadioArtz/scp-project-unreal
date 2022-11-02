

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Attributes/AttributeFloat.h"
#include "PlayerAttributeComponent.generated.h"

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerAttributeComponentValueChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SCPPU_API UPlayerAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

	//// Properties ////
public:	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float CurrentValue; // get private set

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FAttributeFloat MinValue; // get set

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FAttributeFloat MaxValue; // get set

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FAttributeFloat RegenrationRate; // get set

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FAttributeFloat RegenerationDelay; // get set

	UPROPERTY(BlueprintAssignable)
		FPlayerAttributeComponentValueChanged OnMinValueReached;

	UPROPERTY(BlueprintAssignable)
		FPlayerAttributeComponentValueChanged OnMinValueLeft;

	UPROPERTY(BlueprintAssignable)
		FPlayerAttributeComponentValueChanged OnMaxValueReached;

	UPROPERTY(BlueprintAssignable)
		FPlayerAttributeComponentValueChanged OnMaxValueLeft;

private:
		FTimerHandle RegenerationDelayTimerHandle;

		bool bMinValueReached = false;

		bool bMaxValueReached = false;

		bool bDelayRegeneration = false;

	//// Functions ////	
public:
	UFUNCTION(BlueprintCallable)
		void SetCurrentValue(float NewValue, bool bSkipRegenrationDelay = false);

	// Sets default values for this component's properties
	UPlayerAttributeComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
};
