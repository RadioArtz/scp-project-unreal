// 

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "scppu/GameLogic/ScpPuGameStateBase.h"

#include "PlayerInteractionBase.generated.h"

USTRUCT()
struct FPlayerInteractionBaseSecondaryTickFunction : public FActorComponentTickFunction
{
	GENERATED_USTRUCT_BODY()

    /** 
    * Abstract function actually execute the tick. 
    * @param DeltaTime - frame time to advance, in seconds
    * @param TickType - kind of tick for this frame
    * @param CurrentThread - thread we are executing on, useful to pass along as new tasks are created
    * @param MyCompletionGraphEvent - completion event for this task. Useful for holding the completetion of this task until certain child tasks are complete.
    **/
   virtual void ExecuteTick(float DeltaTime, ELevelTick TickType, ENamedThreads::Type CurrentThread, const FGraphEventRef& MyCompletionGraphEvent) override;
};

template<>
struct TStructOpsTypeTraits<FPlayerInteractionBaseSecondaryTickFunction> : public TStructOpsTypeTraitsBase2<FPlayerInteractionBaseSecondaryTickFunction>
{
	enum
	{
		WithCopy = false
    };
};


USTRUCT(BlueprintType)
struct FInteractiveObjectInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	uint8 bIsValid = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UClass* Class = nullptr;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector Location = FVector::ZeroVector;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float PreCalculatedGrabRange = -1;
};



class UInteractionGrab;



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SCPPU_API UPlayerInteractionBase : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPlayerInteractionBase();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void SecondaryTickComponent(float DeltaTime, ELevelTick TickType,
                               FActorComponentTickFunction* ThisTickFunction);

public:
	//Returns closest interactive object, after checking if it's valid and still in range.
	UFUNCTION(BlueprintCallable)
	UInteractionBase* GetClosestInteractiveObject() const;

	//Returns the most important data for closest interactive object,
	//without any additional checks. If some data from object is needed every frame,
	//it should be inside this struct.
	//Returns Empty struct when ClosestInteractiveObject is not valid
	UFUNCTION(BlueprintCallable)
	FInteractiveObjectInfo GetClosestInteractiveObjectInfo() const;

	UFUNCTION(BlueprintCallable)
	UInteractionGrab* GetCurrentlyGrabbedObject() const;

	UFUNCTION(BlueprintCallable)
	bool CanInteractWithObject(const UInteractionBase* Obj) const;

	UFUNCTION(BlueprintCallable)
	bool CanGrabObject(const UInteractionBase* Obj) const;

	UFUNCTION(BlueprintCallable)
	bool InteractionStart();

	UFUNCTION(BlueprintCallable)
	bool InteractionEnd();

protected:

	UFUNCTION(BlueprintCallable)
	bool InteractionGrab(UInteractionGrab* ObjectToGrab);

	UFUNCTION(BlueprintCallable)
	bool InteractionDrop();

	UFUNCTION(BlueprintCallable)
	float CalculateObjectGrabRange(const UInteractionGrab* ObjectToGrab) const;

	UFUNCTION()
	void InternalSearchForNearestInteractiveObject();
	
public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float InteractionRange = 300;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float DefaultGrabRange = 300;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float GrabRange = DefaultGrabRange;

	// Interaction search is locked after call to InteractionStart(), and has to be
	// unlocked by InteractionEnd(). While interaction search is locked, ClosestInteractiveObject
	// cannot be changed to prevent situation, where player clicks on one object, and then moves
	// to another activating InteractionEnd() on it.
	// If Player leaves InteractionRange, while interaction search is locked, InteractionEnd() is
	// called automatically. 
	UPROPERTY(BlueprintReadWrite)
	uint8 bInteractionSearchLocked = false;

	
public:

	UPROPERTY(EditDefaultsOnly, Category="ComponentTick")
	struct FPlayerInteractionBaseSecondaryTickFunction SecondaryComponentTick;

protected:

	UPROPERTY(BlueprintReadOnly, Transient)
	AScpPuGameStateBase* ScpPuGameState;
	
	UPROPERTY(BlueprintReadOnly, Transient)
	TWeakObjectPtr<UInteractionBase> ClosestInteractiveObject;

	UPROPERTY(BlueprintReadOnly, Transient)
	FInteractiveObjectInfo ClosestInteractiveObjectInfo;

	UPROPERTY(BlueprintReadOnly, Transient)
	UInteractionGrab* CurrentlyGrabbedObject;
};
