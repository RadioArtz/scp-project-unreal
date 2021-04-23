

#pragma once

#include "CoreMinimal.h"
#include "InteractionBase.h"
#include "Player/PlayerInteractionBase.h"

#include "InteractionGrab.generated.h"


DECLARE_DYNAMIC_DELEGATE_OneParam(FInteractGrabDeleg, UPlayerInteractionBase*, Caller);
DECLARE_DYNAMIC_DELEGATE_OneParam(FInteractDropDeleg, UPlayerInteractionBase*, Caller);

/**
 *	More complex form of interaction used for,
 *	moving or rotating objects, but not picking them up.
**/
UCLASS( ClassGroup=(Interaction), meta=(BlueprintSpawnableComponent) )
class SCPPU_API UInteractionGrab : public UInteractionBase
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void InteractGrab(UPlayerInteractionBase* Caller);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void InteractDrop(UPlayerInteractionBase* Caller);

	virtual EInteractionType GetInteractionType_Implementation() override;

public:
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FInteractGrabDeleg InteractGrabDelegate;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FInteractDropDeleg InteractDropDelegate;

	//Custom grab range should override grab range in Player Interaction Component
	//Ignored if CustomGrabRange < 0
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CustomGrabRange = -1;

	//Custom grab range Multiplier should multiply grab range in Player Interaction Component
	//Ignored if CustomGrabRange >= 0
	//Ignored if CustomGrabRangeMultiplier < 0
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float CustomGrabRangeMultiplier = -1;

protected:

	UPROPERTY(BlueprintReadOnly, Transient)
	UPlayerInteractionBase* HoldingPlayer = nullptr;
	
};
