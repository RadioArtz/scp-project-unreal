

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly) // get private set
		bool bIsEnabled = true;

	UPROPERTY(BlueprintReadOnly) // get private set
		bool bIsInUse = false;

	UPROPERTY(BlueprintReadOnly) // get private set
		APawn* CurrentInteractor;

	UPROPERTY(BlueprintReadOnly) // get private set
		UObject* CurrentItem;

	//// Functions ////	
public:
	UFUNCTION(BlueprintCallable)
		static TArray<UInteractionComponentBase*> GetInteractionComponentsInRadius(FVector ClosestFrom, float Radius = 350.0f, bool bMustBeReachable = true, FVector ReachableFrom = FVector(0, 0, 0));

	UFUNCTION(BlueprintCallable)
		static UInteractionComponentBase* GetClosestInteractionComponentInRadius(FVector ClosestFrom, float Radius = 350.0f, bool bMustBeReachable = true, FVector ReachableFrom = FVector(0, 0, 0));

	// Sets default values for this component's properties
	UInteractionComponentBase();

	UFUNCTION(BlueprintCallable)
		virtual bool StartInteraction(APawn* Interactor, UObject* Item);

	UFUNCTION(BlueprintCallable)
		virtual bool EndInteraction(APawn* Interactor);

	UFUNCTION(BlueprintCallable)
		virtual void SetIsEnabled(bool bNewIsEnabled);

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called when the game ends or when destroyed
	virtual void BeginDestroy() override;	
};
