

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseItem.generated.h"

class UInventoryComponent;

UCLASS()
class SCPPU_API ABaseItem : public AActor
{
	GENERATED_BODY()

	//// Properties ////
public:
	static TArray<ABaseItem*> RegisteredItems;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FText Name = FText::FromString("Default Item");

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FText Description = FText::FromString("It does nothing and adds nothing to the game.");

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UTexture2D* InventoryTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		USoundBase* InventorySound;

	UPROPERTY(BlueprintReadOnly)
		UInventoryComponent* OwningInventoryComponent = nullptr;
	
	//// Functions ////	
public:
	UFUNCTION(BlueprintCallable)
		static TArray<ABaseItem*> GetItemsInRadius(FVector ClosestFrom, float Radius = 350.0f, bool bMustBeReachable = true, FVector ReachableFrom = FVector(0, 0, 0));

	UFUNCTION(BlueprintCallable)
		static ABaseItem* GetClosestItemInRadius(FVector ClosestFrom, float Radius = 350.0f, bool bMustBeReachable = true, FVector ReachableFrom = FVector(0, 0, 0));


	// Sets default values for this actor's properties
	ABaseItem();

	UFUNCTION()
		void SetOwningInventory(UInventoryComponent* NewOwningInventoryComponent);

	UFUNCTION(BlueprintImplementableEvent)
		void OnAddedToInventory(UInventoryComponent* NewOwningInventoryComponent);

	UFUNCTION(BlueprintImplementableEvent)
		void OnRemovedFromInventory(UInventoryComponent* PreviousOwningInventoryComponent);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the game ends or when destroyed
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
};
