


#include "Inventory/BaseItem.h"
#include "Inventory/InventoryComponent.h"
#include "Layout/Layout.h"
#include "Layout/LayoutCell.h"

TArray<ABaseItem*> ABaseItem::RegisteredItems;

TArray<ABaseItem*> ABaseItem::GetItemsInRadius(FVector ClosestFrom, float Radius, bool bMustBeReachable, FVector ReachableFrom)
{
	TArray<ABaseItem*> Items;
	for (auto& ElemItem : ABaseItem::RegisteredItems)
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

ABaseItem* ABaseItem::GetClosestItemInRadius(FVector ClosestFrom, float Radius, bool bMustBeReachable, FVector ReachableFrom)
{
	ABaseItem* Item = nullptr;
	for (auto& Elem : ABaseItem::GetItemsInRadius(ClosestFrom, Radius, bMustBeReachable, ReachableFrom))
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
ABaseItem::ABaseItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	this->PrimaryActorTick.bCanEverTick = true;
	this->ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>("ItemMesh", false);
	this->ItemMesh->SetMobility(EComponentMobility::Movable);
	this->ItemMesh->SetCollisionProfileName("Item");
	this->SetRootComponent(this->ItemMesh);
	this->SetActorHiddenInGame(false);
	this->SetActorEnableCollision(true);
}

void ABaseItem::SetOwningInventory(UInventoryComponent* NewOwningInventoryComponent)
{
	UInventoryComponent* PrevOwningInventoryComponent = this->OwningInventoryComponent;
	this->OwningInventoryComponent = NewOwningInventoryComponent;

	// Item has been dropped/removed from inventory
	if (this->OwningInventoryComponent == nullptr)
	{
		this->SetActorHiddenInGame(false);
		this->SetActorEnableCollision(true);
		this->ItemMesh->SetSimulatePhysics(true);
		this->ItemMesh->RecreatePhysicsState();
		this->SetOwner(nullptr);
		this->OnRemovedFromInventory(PrevOwningInventoryComponent);
		ABaseItem::RegisteredItems.Add(this);

		// Subscribe to layout cell events to enable/disable physics
		ALayout* Layout;
		ULayoutCell* Cell;
		ALayout::FindLayoutAndCellFromWorldLocation(Layout, Cell, this->GetActorLocation(), 500.f);
		if (IsValid(Layout) && IsValid(Cell))
		{
			this->ItemMesh->SetSimulatePhysics(Cell->IsSublevelVisible());
			this->ItemMesh->RecreatePhysicsState();
			Cell->OnSublevelShown.AddDynamic(this, &ABaseItem::OnEnclosingSublevelShownCallback);
			Cell->OnSublevelHidden.AddDynamic(this, &ABaseItem::OnEnclosingSublevelHiddenCallback);
			this->EnclosingCell = Cell;
		}
	}
	else // Item has been added/moved
	{
		this->SetActorHiddenInGame(true);
		this->SetActorEnableCollision(false);
		this->ItemMesh->SetSimulatePhysics(false);
		this->ItemMesh->RecreatePhysicsState();
		this->SetOwner(this->OwningInventoryComponent->GetOwner());
		this->OnAddedToInventory(this->OwningInventoryComponent);
		ABaseItem::RegisteredItems.Remove(this);

		// Unubscribe from layout cell events
		if (IsValid(this->EnclosingCell))
		{
			this->EnclosingCell->OnSublevelShown.RemoveDynamic(this, &ABaseItem::OnEnclosingSublevelShownCallback);
			this->EnclosingCell->OnSublevelHidden.RemoveDynamic(this, &ABaseItem::OnEnclosingSublevelHiddenCallback);
		}
	}
}

// Called when the game starts or when spawned
void ABaseItem::BeginPlay()
{
	Super::BeginPlay();
	ABaseItem::RegisteredItems.Add(this);
	ABaseItem::SetOwningInventory(nullptr);
}

void ABaseItem::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	if (this->OwningInventoryComponent != nullptr)
	{
		this->OwningInventoryComponent->DropItem(this->OwningInventoryComponent->FindSlotOfItem(this), FVector(0, 0, 0));
	}

	ABaseItem::RegisteredItems.Remove(this);
}

void ABaseItem::OnEnclosingSublevelShownCallback()
{
	this->ItemMesh->SetSimulatePhysics(true);
	this->ItemMesh->RecreatePhysicsState();
}

void ABaseItem::OnEnclosingSublevelHiddenCallback()
{
	this->ItemMesh->SetSimulatePhysics(false);
	this->ItemMesh->RecreatePhysicsState();
}

void ABaseItem::OnLayoutDataReceived_Implementation(ALayout* Layout, ULayoutCell* Cell, FRandomStream RandomStream)
{
	Cell->Sublevel->OnLevelShown.AddDynamic(this, &ABaseItem::OnEnclosingSublevelShownCallback);
	Cell->Sublevel->OnLevelHidden.AddDynamic(this, &ABaseItem::OnEnclosingSublevelHiddenCallback);
	this->EnclosingCell = Cell;
}

// Called every frame
void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

