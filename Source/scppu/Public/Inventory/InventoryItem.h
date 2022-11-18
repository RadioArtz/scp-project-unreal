

#pragma once

#include "CoreMinimal.h"
#include "Inventory/BaseItem.h"
#include "InventoryItem.generated.h"

class UInventoryComponent;

UCLASS()
class SCPPU_API AInventoryItem : public ABaseItem
{
	GENERATED_BODY()

	//// Properties ////
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UInventoryComponent* InventoryComponent;

	//// Functions ////
public:
	AInventoryItem();

};
