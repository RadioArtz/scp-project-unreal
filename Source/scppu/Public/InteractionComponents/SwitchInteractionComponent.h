

#pragma once

#include "CoreMinimal.h"
#include "InteractionComponents/BaseInteractionComponent.h"
#include "SwitchInteractionComponent.generated.h"

class ABaseItem;

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSwitchInteractionComponentBeginInteract, APawn*, Interactor, ABaseItem*, Item);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FSwitchInteractionComponentTickInteract, float, DeltaTime, float, Alpha, int32, State);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSwitchInteractionComponentEndInteract, float, FinalAlpha, int32, FinalState);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSwitchInteractionComponentNewState, float, NewAlpha, int32, NewState);

UENUM(BlueprintType)
enum class ESwitchInteractionComponentMovementMode : uint8
{
	Horizontal,
	Vertical,
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SCPPU_API USwitchInteractionComponent : public UBaseInteractionComponent
{
	GENERATED_BODY()

	//// Properties ////
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1", ClampMax = "32")) // get private set
		int32 CurrentState = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "2", ClampMax = "32")) // get private set
		int32 MaxState = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) // get private set
		ESwitchInteractionComponentMovementMode MovementMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) // get private set
		float Sensitivity = 5.0f;

	UPROPERTY() //private get private set
		FRotator InteractorStartRotation;

	UPROPERTY() //private get private set
		float AlphaOffset;

	UPROPERTY(BlueprintAssignable)
		FSwitchInteractionComponentBeginInteract OnBeginInteract;

	UPROPERTY(BlueprintAssignable)
		FSwitchInteractionComponentTickInteract OnTickInteract;

	UPROPERTY(BlueprintAssignable)
		FSwitchInteractionComponentEndInteract OnEndInteract;

	UPROPERTY(BlueprintAssignable)
		FSwitchInteractionComponentNewState OnStateChanged;

	//// Functions ////	
public:
	UFUNCTION(BlueprintCallable)
		void SetCurrentState(int32 NewState = 1);

	virtual bool BeginInteraction(APawn* Interactor, ABaseItem* Item) override;

	virtual bool EndInteraction(APawn* Interactor) override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
};
