

#include "Utilities/RotatingBoxReflectionCapture.h"
#include "Components/BoxComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/BoxReflectionCaptureComponent.h"
#include "Components/SceneCaptureComponentCube.h"
#include "Engine/TextureRenderTargetCube.h"
#include "Engine/TextureCube.h"
#include "Engine/MapBuildDataRegistry.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY(LogRotatingReflectionCapture);

// Sets default values
ARotatingBoxReflectionCapture::ARotatingBoxReflectionCapture()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	this->PrimaryActorTick.bCanEverTick = false;

	// Root component
	this->Root = CreateDefaultSubobject<USceneComponent >("Root", false);
	this->Root->SetMobility(EComponentMobility::Static);
	this->Root->SetWorldScale3D(FVector(500, 500, 200));
	this->SetRootComponent(this->Root);

#if WITH_EDITOR
	// Sprite component
	this->EditorSprite = CreateDefaultSubobject<UBillboardComponent>("Sprite", false);
	this->EditorSprite->SetSprite(ConstructorHelpers::FObjectFinder<UTexture2D>(TEXT("Texture2D'/Engine/EditorResources/S_ReflActorIcon.S_ReflActorIcon'")).Object);
	this->EditorSprite->bIsScreenSizeScaled = true;
	this->EditorSprite->ScreenSize = 0.0025f;
	this->EditorSprite->SetRelativeLocation(FVector(0, 0, 0));
	this->EditorSprite->AttachToComponent(this->Root, FAttachmentTransformRules::KeepRelativeTransform);
	
	// Box component
	this->EditorBox = CreateDefaultSubobject<UBoxComponent >("Box", false);
	this->EditorBox->SetBoxExtent(FVector(1, 1, 1), false);
	this->EditorBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	this->EditorBox->SetRelativeLocation(FVector(0, 0, 0));
	this->EditorBox->AttachToComponent(this->Root, FAttachmentTransformRules::KeepRelativeTransform);
#endif

	// Reflection capture rotation 0
	this->ReflectionCapture0Deg = CreateDefaultSubobject<UBoxReflectionCaptureComponent>("BoxReflectionCaptureRot0", false);
	this->ReflectionCapture0Deg->ReflectionSourceType = EReflectionSourceType::SpecifiedCubemap;
	this->ReflectionCapture0Deg->SourceCubemapAngle = 0;
	this->ReflectionCapture0Deg->SetRelativeLocation(FVector(0, 0, 0));
	this->ReflectionCapture0Deg->AttachToComponent(this->Root, FAttachmentTransformRules::KeepRelativeTransform);
	this->ReflectionCapture0Deg->SetVisibility(true);

	// Reflection capture rotation 90
	this->ReflectionCapture90Deg = CreateDefaultSubobject<UBoxReflectionCaptureComponent>("BoxReflectionCaptureRot90", false);
	this->ReflectionCapture90Deg->ReflectionSourceType = EReflectionSourceType::SpecifiedCubemap;
	this->ReflectionCapture90Deg->SourceCubemapAngle = 90;
	this->ReflectionCapture90Deg->SetRelativeLocation(FVector(0, 0, 0));
	this->ReflectionCapture90Deg->AttachToComponent(this->Root, FAttachmentTransformRules::KeepRelativeTransform);
	this->ReflectionCapture90Deg->SetVisibility(true);

	// Reflection capture rotation 180
	this->ReflectionCapture180Deg = CreateDefaultSubobject<UBoxReflectionCaptureComponent>("BoxReflectionCaptureRot180", false);
	this->ReflectionCapture180Deg->ReflectionSourceType = EReflectionSourceType::SpecifiedCubemap;
	this->ReflectionCapture180Deg->SourceCubemapAngle = 180;
	this->ReflectionCapture180Deg->SetRelativeLocation(FVector(0, 0, 0));
	this->ReflectionCapture180Deg->AttachToComponent(this->Root, FAttachmentTransformRules::KeepRelativeTransform);
	this->ReflectionCapture180Deg->SetVisibility(true);

	// Reflection capture rotation 270
	this->ReflectionCapture270Deg = CreateDefaultSubobject<UBoxReflectionCaptureComponent>("BoxReflectionCaptureRot270", false);
	this->ReflectionCapture270Deg->ReflectionSourceType = EReflectionSourceType::SpecifiedCubemap;
	this->ReflectionCapture270Deg->SourceCubemapAngle = 270;
	this->ReflectionCapture270Deg->SetRelativeLocation(FVector(0, 0, 0));
	this->ReflectionCapture270Deg->AttachToComponent(this->Root, FAttachmentTransformRules::KeepRelativeTransform);
	this->ReflectionCapture270Deg->SetVisibility(true);

	// Scene capture cube
#if WITH_EDITOR
	this->SceneCaptureCube = CreateDefaultSubobject<USceneCaptureComponentCube>("SceneCaptureCube", false);
	this->SceneCaptureCube->bCaptureEveryFrame = false;
	this->SceneCaptureCube->bCaptureOnMovement = false;
	this->SceneCaptureCube->SetRelativeLocation(FVector(0, 0, 0));
	this->SceneCaptureCube->SetRelativeScale3D(FVector(0.00001, 0.00001, 0.00001));
	this->SceneCaptureCube->AttachToComponent(this->Root, FAttachmentTransformRules::KeepRelativeTransform);
#endif
}

#if WITH_EDITOR
void ARotatingBoxReflectionCapture::UpdateCapture()
{
	if (this->GetWorld()->WorldType != EWorldType::Editor)
	{
		UE_LOG(LogRotatingReflectionCapture, Error, TEXT("%s: Cannot update inside a non-editor world"), *this->GetName());
		return;
	}
	
	// Clear arrays
	this->RenderTargets.Reset(this->NumReflectiveCaputures + 1);
	this->StaticTextures.Reset(this->NumReflectiveCaputures + 1);

	for (int i = 0; i < this->NumReflectiveCaputures + 1; i++)
	{
		// Create new render target
		UE_LOG(LogRotatingReflectionCapture, Log, TEXT("%s: Creating new render target [%i]..."), *this->GetName(), i);
		UTextureRenderTargetCube* RenderTarget = NewObject<UTextureRenderTargetCube>(this, FName(), RF_Transient);
		RenderTarget->ClearColor = FLinearColor::Green;
		RenderTarget->Init(256, EPixelFormat::PF_FloatRGBA);
		RenderTarget->UpdateResourceImmediate(true);

		// Capture scence
		UE_LOG(LogRotatingReflectionCapture, Log, TEXT("%s: Capturing scene [%i]..."), *this->GetName(), i);
		if (i == 0)
		{
			this->SceneCaptureCube->ShowFlags.ReflectionEnvironment = false;
		}
		else
		{
			this->SceneCaptureCube->ShowFlags.ReflectionEnvironment = true;
		}
		this->SceneCaptureCube->TextureTarget = RenderTarget;
		this->SceneCaptureCube->CaptureScene();

		// Create and draw render target to static texture
		UE_LOG(LogRotatingReflectionCapture, Log, TEXT("%s: Creating new static texture [%i]..."), *this->GetName(), i);
		UTextureCube* StaticTexture = NewObject<UTextureCube>(this, FName(), RF_Transient);
		StaticTexture = RenderTarget->ConstructTextureCube(StaticTexture->GetOuter(), StaticTexture->GetName(), RenderTarget->GetMaskedFlags());
		StaticTexture->Modify();
		StaticTexture->MarkPackageDirty();
		StaticTexture->PostEditChange();
		StaticTexture->UpdateResource();

		// Apply new static texture to reflection capture
		this->ReflectionCapture0Deg->Cubemap = StaticTexture;
		this->ReflectionCapture90Deg->Cubemap = StaticTexture;
		this->ReflectionCapture180Deg->Cubemap = StaticTexture;
		this->ReflectionCapture270Deg->Cubemap = StaticTexture;

		// Add render target and static texture to array for debug purposes
		this->RenderTargets.Add(RenderTarget);
		this->StaticTextures.Add(StaticTexture);
	}

	UE_LOG(LogRotatingReflectionCapture, Log, TEXT("%s: Updated reflection captures"), *this->GetName());
}

void ARotatingBoxReflectionCapture::OnConstruction(const FTransform& Transform)
{
	if (this->GetWorld()->WorldType != EWorldType::Editor)
	{
		return;
	}

	// Set brightness level
	this->ReflectionCapture0Deg->Brightness = this->Brightness;
	this->ReflectionCapture90Deg->Brightness = this->Brightness;
	this->ReflectionCapture180Deg->Brightness = this->Brightness;
	this->ReflectionCapture270Deg->Brightness = this->Brightness;

	// If we open the level run defer update by one tick, else defer the update longer to prevent massive render target/static texture creation
	if (this->DeferredUpdateTimer.IsValid())
	{
		this->GetWorld()->GetTimerManager().ClearTimer(this->DeferredUpdateTimer);
	}

	FTimerDelegate Delegate;
	float DeferDelay = this->GetWorld()->UnpausedTimeSeconds < 1.0 ? 0.01f : 0.25f;
	Delegate.BindUObject(this, &ARotatingBoxReflectionCapture::UpdateCapture);
	this->GetWorld()->GetTimerManager().SetTimer(this->DeferredUpdateTimer, Delegate, 0.25f, false);

	this->PlacedRotation = this->GetActorRotation();
}
#endif

// Called when the game starts or when spawned
void ARotatingBoxReflectionCapture::BeginPlay()
{
	Super::BeginPlay();

	if (this->bRemovedUnnecessaryComponents)
	{
		return;
	}

	// Delete reflection captures based on our "new" rotation
	int Rotation = FMath::DivideAndRoundNearest((this->GetActorRotation() - this->PlacedRotation).GetDenormalized().Yaw, 90.0f);
	
	switch (Rotation)
	{
	case 0:
		this->ReflectionCapture0Deg->SetVisibility(true);
		this->ReflectionCapture90Deg->DestroyComponent();
		this->ReflectionCapture180Deg->DestroyComponent();
		this->ReflectionCapture270Deg->DestroyComponent();
		this->bRemovedUnnecessaryComponents = true;
		break;
	case 1:
		this->ReflectionCapture270Deg->SetVisibility(true); //Soruce angle rotates in the opposite direction
		this->ReflectionCapture0Deg->DestroyComponent();
		this->ReflectionCapture90Deg->DestroyComponent();
		this->ReflectionCapture180Deg->DestroyComponent();
		this->bRemovedUnnecessaryComponents = true;
		break;
	case 2:
		this->ReflectionCapture180Deg->SetVisibility(true);
		this->ReflectionCapture270Deg->DestroyComponent();
		this->ReflectionCapture0Deg->DestroyComponent();
		this->ReflectionCapture90Deg->DestroyComponent();
		this->bRemovedUnnecessaryComponents = true;
		break;
	case 3:
		this->ReflectionCapture90Deg->SetVisibility(true); //Soruce angle rotates in the opposite direction
		this->ReflectionCapture180Deg->DestroyComponent();
		this->ReflectionCapture270Deg->DestroyComponent();
		this->ReflectionCapture0Deg->DestroyComponent();
		this->bRemovedUnnecessaryComponents = true;
		break;
	default:
		UE_LOG(LogRotatingReflectionCapture, Error, TEXT("%s: Detected rotation %d is invalid"), *this->GetName(), Rotation);
		return;
	}

	UE_LOG(LogRotatingReflectionCapture, Log, TEXT("%s: Detected rotation %d"), *this->GetName(), Rotation);
}

// Called every frame
void ARotatingBoxReflectionCapture::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

