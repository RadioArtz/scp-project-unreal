

#pragma once

#include "CoreMinimal.h"
#include "InteractionComponents/InteractionComponentBase.h"
#include "InteractionComponentSwitch.generated.h"

class AItemBase;

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionComponentSwitchInteractStart, APawn*, Interactor, AItemBase*, Item);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FInteractionComponentSwitchInteractTick, float, DeltaTime, float, Alpha, int32, State);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionComponentSwitchInteractEnd, float, FinalAlpha, int32, FinalState);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionComponentSwitchNewState, float, NewAlpha, int32, NewState);

UENUM(BlueprintType)
enum class EInteractionComponentSwitchMoveMode : uint8
{
	Horizontal,
	Vertical,
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SCPPU_API UInteractionComponentSwitch : public UInteractionComponentBase
{
	GENERATED_BODY()

	//// Properties ////
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1", ClampMax = "32")) // get private set
		int32 CurrentState = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "2", ClampMax = "32")) // get private set
		int32 MaxState = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) // get private set
		EInteractionComponentSwitchMoveMode MovementMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) // get private set
		float Sensitivity = 5.0f;

	UPROPERTY() //private get private set
		FRotator InteractorStartRotation;

	UPROPERTY() //private get private set
		float AlphaOffset;

	UPROPERTY(BlueprintAssignable)
		FInteractionComponentSwitchInteractStart OnInteractStart;

	UPROPERTY(BlueprintAssignable)
		FInteractionComponentSwitchInteractTick OnInteractTick;

	UPROPERTY(BlueprintAssignable)
		FInteractionComponentSwitchInteractEnd OnInteractEnd;

	UPROPERTY(BlueprintAssignable)
		FInteractionComponentSwitchNewState OnStateChanged;

	//// Functions ////	
public:
	UFUNCTION(BlueprintCallable)
		void SetCurrentState(int NewState = 1);

	virtual bool StartInteraction(APawn* Interactor, AItemBase* Item) override;

	virtual bool EndInteraction(APawn* Interactor) override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
};
