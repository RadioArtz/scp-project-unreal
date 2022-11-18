

#pragma once

#include "CoreMinimal.h"
#include "Inventory/BaseItem.h"
#include "WearableItem.generated.h"

UCLASS()
class SCPPU_API AWearableItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (DefaultToSelf = "Interactor", HidePin = "Interactor"))
		void OnBeginWear(APawn* Interactor);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (DefaultToSelf = "Interactor", HidePin = "Interactor"))
		void OnEndWear(APawn* Interactor);
};
