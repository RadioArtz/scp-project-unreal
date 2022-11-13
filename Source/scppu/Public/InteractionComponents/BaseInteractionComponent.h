

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "BaseInteractionComponent.generated.h"

class ABaseItem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SCPPU_API UBaseInteractionComponent : public USceneComponent
{
	GENERATED_BODY()

	//// Properties ////
public:
	static TArray<UBaseInteractionComponent*> RegisteredInteractionComponents;

	UPROPERTY(EditAnywhere, BlueprintReadOnly) // get private set
		bool bIsEnabled = true;

	UPROPERTY(BlueprintReadOnly) // get private set
		bool bIsInUse = false;

	UPROPERTY(BlueprintReadOnly) // get private set
		APawn* CurrentInteractor;

	UPROPERTY(BlueprintReadOnly) // get private set
		ABaseItem* CurrentItem;

	//// Functions ////	
public:
	UFUNCTION(BlueprintCallable)
		static TArray<UBaseInteractionComponent*> GetInteractionComponentsInRadius(FVector ClosestFrom, float Radius = 350.0f, bool bMustBeReachable = true, FVector ReachableFrom = FVector(0, 0, 0));

	UFUNCTION(BlueprintCallable)
		static UBaseInteractionComponent* GetClosestInteractionComponentInRadius(FVector ClosestFrom, float Radius = 350.0f, bool bMustBeReachable = true, FVector ReachableFrom = FVector(0, 0, 0));

	// Sets default values for this component's properties
	UBaseInteractionComponent();

	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "Interactor", HidePin = "Interactor"))
		virtual bool BeginInteraction(APawn* Interactor, ABaseItem* Item);

	UFUNCTION(BlueprintCallable, meta = (DefaultToSelf = "Interactor", HidePin = "Interactor"))
		virtual bool EndInteraction(APawn* Interactor);

	UFUNCTION(BlueprintCallable)
		virtual void SetEnabled(bool bNewEnabled);

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called when the game ends or when destroyed
	virtual void BeginDestroy() override;
};
