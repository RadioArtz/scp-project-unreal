

#include "Utilities/ViewportCaptureComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Rendering/Texture2DResource.h"
#include "GameFramework/PlayerController.h" 

DEFINE_LOG_CATEGORY(LogViewportCapture);

// Sets default values for this component's properties
UViewportCaptureComponent::UViewportCaptureComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called every frame
void UViewportCaptureComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	FTransform NewActiveCameraTransform = this->GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetTransform();

	if (this->bCaptureEveryFrame)
	{
		this->CaptureViewportDeferred();
	}
	else if (this->bCaptureOnMovement && !(this->LastActiveCameraTransform.Equals(NewActiveCameraTransform, .01f)))
	{
		this->CaptureViewportDeferred();
		this->LastActiveCameraTransform = NewActiveCameraTransform;
	}
}

void UViewportCaptureComponent::CaptureViewportDeferred()
{
	if (this->TextureTarget == nullptr)
	{
		UE_LOG(LogViewportCapture, Error, TEXT("%s: Invalid texture target, aborting..."), *this->GetName());
		return;
	}

	if (this->ViewportTextureRHI == nullptr)
	{
		UE_LOG(LogViewportCapture, Error, TEXT("%s: No viewport texture found, aborting..."), *this->GetName());
		return;
	}

	if (this->TextureTarget->GetFormat() != this->ViewportTextureRHI->GetFormat())
	{
		UE_LOG(LogViewportCapture, Error, TEXT("%s: Pixel format of target texture does not match viewport texture (is '%s', should be '%s'), aborting..."), *this->GetName(), *UEnum::GetValueAsString(this->TextureTarget->GetFormat()), *UEnum::GetValueAsString(this->ViewportTextureRHI->GetFormat()));
		return;
	}

	// Copy viewport texture to texture target
	FRHITexture2D* TargetTextureRHILambda = this->TextureTarget->GetResource()->GetTexture2DRHI();
	FRHITexture2D* ViewportTextureRHILambda = this->ViewportTextureRHI;
	FString ComponentNameLambda = this->GetName();
	ENQUEUE_RENDER_COMMAND(CopyViewportTexture)(
		[ViewportTextureRHILambda, TargetTextureRHILambda, ComponentNameLambda](FRHICommandListImmediate& RHICmdList)
		{
			if (TargetTextureRHILambda == nullptr || !TargetTextureRHILambda->IsValid())
			{
				UE_LOG(LogViewportCapture, Error, TEXT("%s: Invalid texture target on render thread (did the texture change?), aborting..."), *ComponentNameLambda);
				return;
			}

			if (ViewportTextureRHILambda == nullptr || !ViewportTextureRHILambda->IsValid())
			{
				UE_LOG(LogViewportCapture, Error, TEXT("%s: Invalid viewport texture on render thread (did the texture change?), aborting..."), *ComponentNameLambda);
				return;
			}

			if (ViewportTextureRHILambda->GetSizeXY() != TargetTextureRHILambda->GetSizeXY())
			{
				UE_LOG(LogViewportCapture, Error, TEXT("%s: Texture size did not match on render thread, aborting..."), *ComponentNameLambda);
				return;
			}

			RHICmdList.TransitionResource(ERHIAccess::CopySrc, ViewportTextureRHILambda);
			RHICmdList.TransitionResource(ERHIAccess::CopyDest, TargetTextureRHILambda);
			RHICmdList.CopyToResolveTarget(ViewportTextureRHILambda, TargetTextureRHILambda, FResolveParams());
			RHICmdList.TransitionResource(ERHIAccess::ReadOnlyMask, ViewportTextureRHILambda);
			RHICmdList.TransitionResource(ERHIAccess::ReadOnlyMask, TargetTextureRHILambda);
		});

	UE_LOG(LogViewportCapture, Verbose, TEXT("%s: Drawn viewport texture to texture target."), *this->GetName());
}

void UViewportCaptureComponent::CaptureViewport()
{
	if (this->bCaptureEveryFrame)
	{
		UE_LOG(LogViewportCapture, Warning, TEXT("%s: Updating while bCaptureEveryFrame is enabled - major inefficiency."), *this->GetName());
	}

	this->CaptureViewportDeferred();
	FlushRenderingCommands();
}

// Called when the game starts
void UViewportCaptureComponent::BeginPlay()
{
	Super::BeginPlay();

	// Don't execute for default objects etc.
	if (!IsValid(this->GetOwner()))
	{
		return;
	}

	if (FSlateApplication::IsInitialized())
	{
		this->OnBackBufferReadyToPresentHandle = FSlateApplication::Get().GetRenderer()->OnBackBufferReadyToPresent().AddUObject(this, &UViewportCaptureComponent::OnBackBufferReadyToPresentCallback);
	}
}

void UViewportCaptureComponent::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Don't execute for default objects etc.
	if (!IsValid(this->GetOwner()))
	{
		return;
	}

	if (FSlateApplication::IsInitialized())
	{
		FSlateApplication::Get().GetRenderer()->OnBackBufferReadyToPresent().Remove(this->OnBackBufferReadyToPresentHandle);
		this->ViewportTextureRHI = nullptr;
	}
}

void UViewportCaptureComponent::OnBackBufferReadyToPresentCallback(SWindow& SlateWindow, const FTexture2DRHIRef& Backbuffer)
{
	if (SlateWindow.IsActive())
	{
		this->ViewportTextureRHI = Backbuffer;
		this->TextureTarget->ResizeTarget(this->ViewportTextureRHI->GetSizeX(), this->ViewportTextureRHI->GetSizeY());
		UE_LOG(LogViewportCapture, Verbose, TEXT("%s: Updated viewport texture pointer to new back buffer of slate."), *this->GetName());
	}
}
