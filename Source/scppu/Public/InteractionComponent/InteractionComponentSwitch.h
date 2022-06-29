

#pragma once

#include "CoreMinimal.h"
#include "InteractionComponent/InteractionComponentBase.h"
#include "InteractionComponentSwitch.generated.h"

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionComponentSwitchInteractStart, APawn*, Interactor, UObject*, Item);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FInteractionComponentSwitchInteractTick, float, DeltaTime, float, Alpha, int32, State);

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractionComponentSwitchInteractEnd, float, FinalAlpha, int32, FinalState);

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "1", ClampMax = "8"))
		int32 CurrentState = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = "2", ClampMax = "10"))
		int32 MaxState = 2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		EInteractionComponentSwitchMoveMode MovementMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float Sensitivity = 5.0f;

	UPROPERTY()
		FRotator InteractorStartRotation;

	UPROPERTY()
		float AlphaOffset;

	UPROPERTY(BlueprintAssignable)
		FInteractionComponentSwitchInteractStart OnInteractStart;

	UPROPERTY(BlueprintAssignable)
		FInteractionComponentSwitchInteractTick OnInteractTick;

	UPROPERTY(BlueprintAssignable)
		FInteractionComponentSwitchInteractEnd OnInteractEnd;

	//// Functions ////	
public:

	virtual void StartInteraction(APawn* Interactor, UObject* Item) override;

	virtual void EndInteraction(APawn* Interactor) override;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
};
