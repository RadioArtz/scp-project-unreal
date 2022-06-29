

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "InteractionComponentBase.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SCPPU_API UInteractionComponentBase : public USceneComponent
{
	GENERATED_BODY()

	//// Properties ////
public:
	static TArray<UInteractionComponentBase*> RegisteredInteractionComponents;

	UPROPERTY(BlueprintReadOnly)
		bool bIsInUse;

	UPROPERTY(BlueprintReadOnly)
		APawn* CurrentInteractor;

	UPROPERTY(BlueprintReadOnly)
		UObject* CurrentItem;

	//// Functions ////	
public:
	UFUNCTION(BlueprintCallable)
		static TArray<UInteractionComponentBase*> GetInteractionComponentsInRadius(FVector WorldLocation, float Radius = 350.0f, bool bMustBeReachable = true);

	UFUNCTION(BlueprintCallable)
		static UInteractionComponentBase* GetClosestInteractionComponentInRadius(FVector WorldLocation, float Radius = 350.0f, bool bMustBeReachable = true);

	// Sets default values for this component's properties
	UInteractionComponentBase();

	UFUNCTION(BlueprintCallable)
		virtual void StartInteraction(APawn* Interactor, UObject* Item);

	UFUNCTION(BlueprintCallable)
		virtual void StopInteraction(APawn* Interactor);

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called when the game ends or when destroyed
	virtual void BeginDestroy() override;

		
};
