


#include "Inventory/InventoryItem.h"
#include "Inventory/InventoryComponent.h"

AInventoryItem::AInventoryItem()
{
	this->InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("Inventory", false);
}
