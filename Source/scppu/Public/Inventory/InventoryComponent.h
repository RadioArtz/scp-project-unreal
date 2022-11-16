

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

	UPROPERTY()
		TMap<int32, ABaseItem*> ItemMap;

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
		ABaseItem* GetItem(int32 Slot);

	UFUNCTION(BlueprintCallable)
		bool MoveItem(int32 FromSlot, UInventoryComponent* ReceivingTarget, int32 ToSlot, bool bSwapIfNecessary);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		int FindSlotOfItem(ABaseItem* Item);

	UFUNCTION(BlueprintCallable)
		void Resize(int32 NewSize, bool bDropExcessiveItems = true, FVector DropLocation = FVector(0, 0, 0));

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsSlotEmpty(int32 Slot);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		int GetFirstEmptySlot();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsItemAllowed(ABaseItem* Item);

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool IsFull();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

		
};
