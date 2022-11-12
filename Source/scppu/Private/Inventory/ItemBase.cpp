


#include "Inventory/ItemBase.h"
#include "Inventory/InventoryComponent.h"

TArray<AItemBase*> AItemBase::RegisteredItems;

TArray<AItemBase*> AItemBase::GetItemsInRadius(FVector ClosestFrom, float Radius, bool bMustBeReachable, FVector ReachableFrom)
{
	TArray<AItemBase*> Items;
	for (auto& ElemItem : AItemBase::RegisteredItems)
	{
		if (FVector::Distance(ClosestFrom, ElemItem->GetActorLocation()) <= Radius)
		{
			if (bMustBeReachable)
			{
				TArray<FHitResult> HitResults;
				FCollisionQueryParams CollisionParams;
				CollisionParams.TraceTag = FName("InventorySystem");
				CollisionParams.bTraceComplex = true;
				ElemItem->GetWorld()->LineTraceMultiByChannel(HitResults, ElemItem->GetActorLocation(), ReachableFrom, ECollisionChannel::ECC_Visibility, CollisionParams);
				bool bAnyValidBlockingHits = false;
				for (auto& ElemHit : HitResults)
				{
					if (ElemHit.IsValidBlockingHit())
					{
						bAnyValidBlockingHits = true;
					}
				}

				if (!bAnyValidBlockingHits)
				{
					Items.Add(ElemItem);
				}
			}
			else
			{
				Items.Add(ElemItem);
			}
		}
	}

	return Items;
}

AItemBase* AItemBase::GetClosestItemInRadius(FVector ClosestFrom, float Radius, bool bMustBeReachable, FVector ReachableFrom)
{
	AItemBase* Item = nullptr;
	for (auto& Elem : AItemBase::GetItemsInRadius(ClosestFrom, Radius, bMustBeReachable, ReachableFrom))
	{
		// (ignore warning, IsValid() checks for nullptr)
		if (IsValid(Item))
		{
			if (FVector::Distance(ClosestFrom, Elem->GetActorLocation()) < FVector::Distance(ClosestFrom, Item->GetActorLocation()))
			{
				Item = Elem;
			}
		}
		else
		{
			Item = Elem;
		}
	}

	return Item;
}

// Sets default values
AItemBase::AItemBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	this->PrimaryActorTick.bCanEverTick = true;
	this->ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>("ItemMesh", false);
	this->ItemMesh->SetMobility(EComponentMobility::Movable);
	this->ItemMesh->SetSimulatePhysics(true);
	this->ItemMesh->SetCollisionProfileName("Item");
	this->SetRootComponent(this->ItemMesh);
	this->SetActorHiddenInGame(false);
	this->SetActorEnableCollision(true);
}

void AItemBase::SetOwningInventory(UInventoryComponent* NewOwningInventoryComponent)
{
	UInventoryComponent* PrevOwningInventoryComponent = this->OwningInventoryComponent;
	this->OwningInventoryComponent = NewOwningInventoryComponent;

	// Item has been dropped
	if (this->OwningInventoryComponent == nullptr)
	{
		this->SetActorHiddenInGame(false);
		this->SetActorEnableCollision(true);
		this->ItemMesh->SetSimulatePhysics(true);
		this->ItemMesh->RecreatePhysicsState();
		this->SetOwner(nullptr);
		this->OnItemDropped(PrevOwningInventoryComponent);
		AItemBase::RegisteredItems.Add(this);
	}
	else // Item has been added/moved
	{
		this->SetActorHiddenInGame(true);
		this->SetActorEnableCollision(false);
		this->ItemMesh->SetSimulatePhysics(false);
		this->SetOwner(this->OwningInventoryComponent->GetOwner());
		this->OnItemAdded(this->OwningInventoryComponent);
		AItemBase::RegisteredItems.Remove(this);
	}
}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	AItemBase::RegisteredItems.Add(this);
}

void AItemBase::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (this->OwningInventoryComponent != nullptr)
	{
		this->OwningInventoryComponent->DropItem(this->OwningInventoryComponent->FindSlotOfItem(this), FVector(0, 0, 0));
	}

	AItemBase::RegisteredItems.Remove(this);
}

// Called every frame
void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

