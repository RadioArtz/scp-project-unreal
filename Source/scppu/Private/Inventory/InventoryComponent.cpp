


#include "Inventory/InventoryComponent.h"
#include "Inventory/ItemBase.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	this->PrimaryComponentTick.bCanEverTick = false;
}

bool UInventoryComponent::AddItem(AItemBase* Item, int Slot)
{
	if (Slot > this->Size || Slot < 0)
	{
		return false;
	}

	if (!this->IsSlotEmpty(Slot))
	{
		return false;
	}

	if (!this->CanItemBeAdded(Item))
	{
		return false;
	}

	Item->AttachToActor(this->GetOwner(), FAttachmentTransformRules::KeepWorldTransform);
	Item->SetActorLocation(this->GetOwner()->GetActorLocation());
	Item->SetOwningInventory(this);
	this->ItemMap.Add(Slot, Item);
	this->OnInventoryChanged.Broadcast();
	return true;
}

bool UInventoryComponent::DropItem(int Slot, FVector DropLocation)
{
	if (Slot > this->Size || Slot < 0)
	{
		return false;
	}

	if (this->IsSlotEmpty(Slot))
	{
		return false;
	}

	AItemBase* Item = this->ItemMap[Slot];
	Item->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Item->SetActorLocation(DropLocation);
	Item->SetOwningInventory(nullptr);
	this->ItemMap.Remove(Slot);
	this->OnInventoryChanged.Broadcast();
	return true;
}

AItemBase* UInventoryComponent::GetItem(int Slot)
{
	if (Slot > this->Size || Slot < 0)
	{
		return nullptr;
	}

	if (this->IsSlotEmpty(Slot))
	{
		return nullptr;
	}

	return this->ItemMap[Slot];
}

bool UInventoryComponent::MoveItem(int FromSlot, UInventoryComponent* ReceivingTarget, int ToSlot, bool bSwapIfNecessary)
{
	if (ReceivingTarget == nullptr)
	{
		ReceivingTarget = this;
	}

	if (FromSlot > this->Size || FromSlot < 0 || ToSlot > ReceivingTarget->Size || ToSlot < 0)
	{
		return false;
	}

	if (this->IsSlotEmpty(FromSlot))
	{
		return false;
	}

	if (!bSwapIfNecessary && !ReceivingTarget->IsSlotEmpty(ToSlot))
	{
		return false;
	}

	AItemBase* MovingItem = this->ItemMap[FromSlot];
	this->ItemMap.Remove(FromSlot);
	if (!ReceivingTarget->IsSlotEmpty(ToSlot))
	{
		this->ItemMap.Add(FromSlot, ReceivingTarget->ItemMap[ToSlot]);
		ReceivingTarget->ItemMap.Remove(ToSlot);
	}

	ReceivingTarget->ItemMap.Add(ToSlot, MovingItem);
	this->OnInventoryChanged.Broadcast();
	
	if (this != ReceivingTarget)
	{
		ReceivingTarget->OnInventoryChanged.Broadcast();
	}

	return true;
}

int UInventoryComponent::FindSlotOfItem(AItemBase* Item)
{
	for (auto Kvp : this->ItemMap)
	{
		if (Kvp.Value == Item)
		{
			return Kvp.Key;
		}
	}

	return -1;
}

void UInventoryComponent::Resize(int NewSize, bool bDropExcessiveItems, FVector DropLocation)
{
	int PrevSize = this->Size;
	this->Size = NewSize;

	if (bDropExcessiveItems && this->Size < PrevSize)
	{
		for (int i = this->Size - 1; i < PrevSize; i++)
		{
			if (!IsSlotEmpty(i))
			{
				if (this->GetFirstEmptySlot() == -1)
				{
					this->DropItem(i, DropLocation);
				}

				this->MoveItem(i, this, this->GetFirstEmptySlot(), false);
			}
		}
	}

	this->OnInventoryChanged.Broadcast();
}

bool UInventoryComponent::IsSlotEmpty(int Slot)
{
	return !this->ItemMap.Contains(Slot);
}

int UInventoryComponent::GetFirstEmptySlot()
{
	for (int i = 0; i < this->Size; i++)
	{
		if (this->IsSlotEmpty(i))
		{
			return i;
		}
	}

	return -1;
}

bool UInventoryComponent::CanItemBeAdded(AItemBase* Item)
{
	if (this->GetFirstEmptySlot() == -1)
	{
		return false;
	}

	if (bUseWhitelistAsBlacklist)
	{
		for (auto Elem : this->Whitelist)
		{
			if (Item->IsA(Elem))
			{
				return false;
			}
		}
	}
	else
	{
		for (auto Elem : this->Whitelist)
		{
			if (!Item->IsA(Elem))
			{
				return false;
			}
		}
	}

	return true;
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

