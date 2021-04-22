#pragma once

#include "Core.h"
#include "Components/ActorComponent.h"
#include "Engine/Texture2D.h"

#include "InteractionBase.generated.h"


//Interaction Type tells us when, and what function should we call.
UENUM(Blueprintable,BlueprintType)
enum class EInteractionType : uint8
{
	//Interactions can be forced if desired.
	NoInteraction UMETA(DisplayName = "NoInteraction"),
	
    /**
    *	Used with @class UInteractionSimple
    *	@func	Interact() should be called when key is pressed
    **/
    TriggerOnStart UMETA(DisplayName = "TriggerOnStart"),

    /**
    *	Used with @class UInteractionSimple
    *	@func	Interact() should be called when key is released
    **/
    TriggerOnEnd UMETA(DisplayName = "TriggerOnEnd"),

    /**
    *	Used with @class UInteractionSimple
    *	@func	Interact() should be called when key is pressed or released
    **/
    TriggerEverytime UMETA(DisplayName = "TriggerEverytime"),

    /**
    *	Used with @class UInteractionGrab
    *	@func	InteractGrab() should be called when key is pressed.
    *	@func	InteractDrop() should be called when key is released or when caller
    *			due to some external factors has to drop the object
    *			e.g. object is too far away or player is damaged
    **/
    InteractionGrab UMETA(DisplayName = "InteractionGrab"),
};



UCLASS( ClassGroup=(Interaction), meta=(BlueprintSpawnableComponent) )
class SCPPU_API UInteractionBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractionBase();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

	//Returns Interact Type, which tells when, and what function should be called.
	//It's just a hint, if we want to force call some interaction, we can do it.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
    EInteractionType GetInteractionType();

	UFUNCTION(BlueprintCallable)
	UTexture2D* GetInteractionImage();
		
};
