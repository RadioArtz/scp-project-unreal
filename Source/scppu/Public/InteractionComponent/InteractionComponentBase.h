

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

	//// Functions ////	
public:
	UFUNCTION(BlueprintCallable)
		static TArray<UInteractionComponentBase*> GetInteractionComponentsInRadius(FVector WorldLocation, float Radius);

	UFUNCTION(BlueprintCallable)
		static UInteractionComponentBase* GetNearestInteractionComponentInRadius(FVector WorldLocation, float Radius);

	// Sets default values for this component's properties
	UInteractionComponentBase();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;	

		
};
