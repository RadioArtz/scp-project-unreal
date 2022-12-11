

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class ABaseItem;

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInventoryComponentChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent, DisplayName = "Inventory Component"))
class SCPPU_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

	//// Properties ////
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		int32 Size = 10;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bUseWhitelistAsBlacklist = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TArray<TSubclassOf<ABaseItem>> Whitelist;

	// Array size is always equal with inventory size. Empty slots hold a nullptr reference.
	UPROPERTY()
		TArray<ABaseItem*> ItemArray;

	UPROPERTY(BlueprintAssignable)
		FInventoryComponentChanged OnInventoryChanged;

	//// Functions ////	
public:
	// Sets default values for this component's properties
	UInventoryComponent();

	UFUNCTION(BlueprintCallable)
		bool AddItem(ABaseItem* Item, int32 Slot);

	UFUNCTION(BlueprintCallable)
		bool DropItem(int32 Slot, FVector DropLocation);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE bool IsValidSlot(int32 Slot) { return Slot >= 0 && Slot < this->Size; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE ABaseItem* GetItem(int32 Slot) { return this->ItemArray.IsValidIndex(Slot) ? this->ItemArray[Slot] : nullptr; }

	UFUNCTION(BlueprintCallable)
		bool MoveItem(int32 FromSlot, UInventoryComponent* ReceivingTarget, int32 ToSlot, bool bSwapIfNecessary);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE int32 FindSlotOfItem(ABaseItem* Item) { return this->ItemArray.Find(Item); }

	UFUNCTION(BlueprintCallable)
		void Resize(int32 NewSize, FVector ExcessiveItemsDropLocation);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		FORCEINLINE bool IsSlotEmpty(int32 Slot) { return (this->GetItem(Slot) == nullptr); }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		int32 GetFirstEmptySlot();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool DoesAcceptItemClass(TSubclassOf<ABaseItem> ItemClass);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsFull();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

		
};
