

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "scppu/Interaction/InteractionBase.h"

#include "ScpPuGameStateBase.generated.h"

/**
 * 
 */
UCLASS()
class SCPPU_API AScpPuGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Transient)
	TArray<UInteractionBase*> ActiveInteractables;
};
