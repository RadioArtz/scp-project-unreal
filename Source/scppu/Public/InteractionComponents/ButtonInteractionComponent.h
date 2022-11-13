

#pragma once

#include "CoreMinimal.h"
#include "InteractionComponents/BaseInteractionComponent.h"
#include "ButtonInteractionComponent.generated.h"

class ABaseItem;

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FButtonInteractionComponentIntertact, APawn*, Interactor, ABaseItem*, Item);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SCPPU_API UButtonInteractionComponent : public UBaseInteractionComponent
{
	GENERATED_BODY()
	
	//// Properties ////
public:
	UPROPERTY(BlueprintAssignable)
		FButtonInteractionComponentIntertact OnInteract;

	//// Functions ////	
public:
		virtual bool StartInteraction(APawn* Interactor, ABaseItem* Item) override;

		virtual bool EndInteraction(APawn* Interactor) override;
};
