

#pragma once

#include "CoreMinimal.h"
#include "Inventory/BaseItem.h"
#include "ConsumableItem.generated.h"

UCLASS()
class SCPPU_API AConsumableItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (DefaultToSelf = "Interactor", HidePin = "Interactor"))
		void OnConsumed(APawn* Interactor);
};
