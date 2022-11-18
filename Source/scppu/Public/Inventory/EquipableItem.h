

#pragma once

#include "CoreMinimal.h"
#include "Inventory/BaseItem.h"
#include "EquipableItem.generated.h"

UCLASS()
class SCPPU_API AEquipableItem : public ABaseItem
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (DefaultToSelf = "Interactor", HidePin = "Interactor"))
		void OnEquiped(APawn* Interactor);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta = (DefaultToSelf = "Interactor", HidePin = "Interactor"))
		void OnUnequiped(APawn* Interactor);
};
