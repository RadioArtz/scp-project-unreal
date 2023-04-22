

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ViewportCaptureComponent.generated.h"

class UTextureRenderTarget2D;

DECLARE_LOG_CATEGORY_EXTERN(LogViewportCapture, Log, All);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SCPPU_API UViewportCaptureComponent : public UActorComponent
{
	GENERATED_BODY()

	//// Properties ////
public:

	/** Whether to update the capture's contents every frame.  If disabled, the component will render once on load and then only when moved. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ViewportCapture)
		bool bCaptureEveryFrame = true;

	/** Whether to update the capture's contents on movement of the active camera.  Disable if you are going to capture manually from blueprint. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ViewportCapture)
		bool bCaptureOnMovement = true;

	/** Output render target of the scene capture that can be read in materals. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ViewportCapture)
		UTextureRenderTarget2D* TextureTarget;

private:
	FRHITexture2D* ViewportTextureRHI;
	FDelegateHandle OnBackBufferReadyToPresentHandle;

	FTransform LastActiveCameraTransform;

	//// Functions ////	
public:	
	// Sets default values for this component's properties
	UViewportCaptureComponent();

	/** Captures the viewport and draws it to the texture target next time the frame gets drawn */
	UFUNCTION(BlueprintCallable, Category = "Rendering|ViewportCapture")
		void CaptureViewportDeferred();

	/** Immediatly captures the viewport and draws it to the texture target (SLOW OPERATION!)*/
	UFUNCTION(BlueprintCallable, Category = "Rendering|ViewportCapture")
		void CaptureViewport();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called when component gets destroyed
	virtual void BeginDestroy() override;

private:
	void OnBackBufferReadyToPresentCallback(SWindow& SlateWindow, const FTexture2DRHIRef& Backbuffer);
};
