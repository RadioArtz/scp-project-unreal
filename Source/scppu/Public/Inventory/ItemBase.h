

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

class UInventoryComponent;

UCLASS()
class SCPPU_API AItemBase : public AActor
{
	GENERATED_BODY()

	//// Properties ////
public:
	static TArray<AItemBase*> RegisteredItems;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		UStaticMeshComponent* ItemMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Name = FText::FromString("Default Item");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Description = FText::FromString("It does nothing and adds nothing to the game.");

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UTexture2D* InventoryTexture;

	UPROPERTY(BlueprintReadOnly)
		UInventoryComponent* OwningInventoryComponent = nullptr;
	
	//// Functions ////	
public:
	UFUNCTION(BlueprintCallable)
		static TArray<AItemBase*> GetItemsInRadius(FVector ClosestFrom, float Radius = 350.0f, bool bMustBeReachable = true, FVector ReachableFrom = FVector(0, 0, 0));

	UFUNCTION(BlueprintCallable)
		static AItemBase* GetClosestItemInRadius(FVector ClosestFrom, float Radius = 350.0f, bool bMustBeReachable = true, FVector ReachableFrom = FVector(0, 0, 0));


	// Sets default values for this actor's properties
	AItemBase();

	UFUNCTION()
		void SetOwningInventory(UInventoryComponent* NewOwningInventoryComponent);

	UFUNCTION(BlueprintImplementableEvent)
		void OnItemAdded(UInventoryComponent* NewOwningInventoryComponent);

	UFUNCTION(BlueprintImplementableEvent)
		void OnItemDropped(UInventoryComponent* PreviousOwningInventoryComponent);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void OnInteract(APawn* Interactor);

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when the game ends or when destroyed
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
};
