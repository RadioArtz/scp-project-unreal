#pragma once

#include "CoreMinimal.h"
#include "InteractionBase.h"
#include "Player/PlayerInteractionBase.h"

#include "InteractionSimple.generated.h"


DECLARE_DYNAMIC_DELEGATE_OneParam(FInteractDeleg, UPlayerInteractionBase*, Caller);


/**
 * The simplest way to interact with object, used for picking up items, or triggering switches.
**/
UCLASS( ClassGroup=(Interaction), meta=(BlueprintSpawnableComponent) )
class SCPPU_API UInteractionSimple : public UInteractionBase
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable)
	void Interact(UPlayerInteractionBase* Caller = nullptr);

	UFUNCTION(BlueprintCallable)
    bool SetInteractionType(EInteractionType IType);

	virtual EInteractionType GetInteractionType_Implementation() override;

public:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FInteractDeleg InteractDelegate;
	
protected:

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	EInteractionType InteractionType = EInteractionType::TriggerOnStart;
};
