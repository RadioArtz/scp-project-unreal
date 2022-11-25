


#include "Inventory/InventoryComponent.h"
#include "Inventory/BaseItem.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	this->PrimaryComponentTick.bCanEverTick = false;
}

bool UInventoryComponent::AddItem(ABaseItem* Item, int32 Slot)
{
	if (!this->IsValidSlot(Slot))
	{
		return false;
	}

	if (!this->IsSlotEmpty(Slot))
	{
		return false;
	}

	if (!this->DoesAcceptItemClass(Item->StaticClass()))
	{
		return false;
	}

	Item->AttachToActor(this->GetOwner(), FAttachmentTransformRules::KeepWorldTransform);
	Item->SetActorLocation(this->GetOwner()->GetActorLocation());
	Item->SetOwningInventory(this);
	this->ItemArray[Slot] = Item;
	this->OnInventoryChanged.Broadcast();
	return true;
}

bool UInventoryComponent::DropItem(int32 Slot, FVector DropLocation)
{
	if (!this->IsValidSlot(Slot))
	{
		return false;
	}

	if (this->IsSlotEmpty(Slot))
	{
		return false;
	}

	ABaseItem* Item = this->GetItem(Slot);
	Item->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Item->SetActorLocation(DropLocation);
	Item->SetOwningInventory(nullptr);
	this->ItemArray[Slot] = nullptr;
	this->OnInventoryChanged.Broadcast();
	return true;
}

bool UInventoryComponent::MoveItem(int32 FromSlot, UInventoryComponent* ReceivingTarget, int32 ToSlot, bool bSwapIfNecessary)
{
	if (ReceivingTarget == nullptr)
	{
		ReceivingTarget = this;
	}

	if (!this->IsValidSlot(FromSlot) || !ReceivingTarget->IsValidSlot(ToSlot))
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

	if (!ReceivingTarget->DoesAcceptItemClass(this->GetItem(FromSlot)->StaticClass()))
	{
		return false;
	}

	if (!ReceivingTarget->IsSlotEmpty(ToSlot) && !this->DoesAcceptItemClass(ReceivingTarget->GetItem(FromSlot)->StaticClass()))
	{
		return false;
	}

	ABaseItem* ItemA = this->GetItem(FromSlot);
	ABaseItem* ItemB = ReceivingTarget->GetItem(ToSlot);
	this->DropItem(FromSlot, FVector::ZeroVector); // Remove item A from us

	if (ItemB != nullptr) // We need to swap 
	{
		ReceivingTarget->DropItem(ToSlot, FVector::ZeroVector); // Remove item B from other inventory
		this->AddItem(ItemB, FromSlot); // Add item B to us
	}

	ReceivingTarget->AddItem(ItemA, ToSlot); // Add item A to other inventory
	this->OnInventoryChanged.Broadcast();

	if (this != ReceivingTarget)
	{
		ReceivingTarget->OnInventoryChanged.Broadcast();
	}

	return true;
}

void UInventoryComponent::Resize(int32 NewSize, FVector ExcessiveItemsDropLocation)
{
	if (NewSize < 0)
	{
		return;
	}

	int32 OriginalSize = this->Size;

	for (int i = 0; i < NewSize; i++)
	{
		if (i >= this->ItemArray.Num())
		{
			this->ItemArray.Add(nullptr);
		}
	}

	for (int i = OriginalSize - 1; i >= NewSize; i--)
	{
		if (this->ItemArray[i] != nullptr)
		{
			if (this->GetFirstEmptySlot() != -1)
			{
				this->MoveItem(i, this, this->GetFirstEmptySlot(), false);	
			}
			else
			{
				this->DropItem(i, ExcessiveItemsDropLocation);
			}
		}

		this->ItemArray.RemoveAt(i);
	}

	// Has to be down here because Size will effect i.a. MoveItem and DropItem functionality
	this->Size = NewSize;
	this->OnInventoryChanged.Broadcast();
}

int32 UInventoryComponent::GetFirstEmptySlot()
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

bool UInventoryComponent::DoesAcceptItemClass(TSubclassOf<ABaseItem> ItemClass)
{
	if (bUseWhitelistAsBlacklist)
	{
		for (auto Elem : this->Whitelist)
		{
			if (ItemClass->IsChildOf(Elem))
			{
				return false;
			}
		}
	}
	else
	{
		for (auto Elem : this->Whitelist)
		{
			if (!ItemClass->IsChildOf(Elem))
			{
				return false;
			}
		}
	}

	return true;
}

bool UInventoryComponent::IsFull()
{
	for (int i = 0; i < this->Size; i++)
	{
		if (this->GetItem(i) == nullptr)
		{
			return false;
		}
	}

	return true;
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	this->ItemArray.Init(nullptr, this->Size);
}

// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

