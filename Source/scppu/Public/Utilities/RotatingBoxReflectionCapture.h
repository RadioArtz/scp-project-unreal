

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RotatingBoxReflectionCapture.generated.h"

class USceneComponent;
class UBillboardComponent;
class UBoxComponent;
class UBoxReflectionCaptureComponent;
class USceneCaptureComponentCube;
class UTextureRenderTargetCube;

DECLARE_LOG_CATEGORY_EXTERN(LogRotatingRefCap, Log, All);

UCLASS(hidecategories = (Collision, Attachment, Actor))
class SCPPU_API ARotatingBoxReflectionCapture : public AActor
{
	GENERATED_BODY()
	
	//// Properties ////
public:
	UPROPERTY()
		USceneComponent* Root;

	UPROPERTY()
		UBillboardComponent* Sprite;

	UPROPERTY()
		UBoxComponent* Box;

	UPROPERTY()
		UBoxReflectionCaptureComponent* RefCap0;

	UPROPERTY()
		UBoxReflectionCaptureComponent* RefCap90;

	UPROPERTY()
		UBoxReflectionCaptureComponent* RefCap180;

	UPROPERTY()
		UBoxReflectionCaptureComponent* RefCap270;

	UPROPERTY()
		USceneCaptureComponentCube* SceneCaptureCube;

	UPROPERTY(EditAnywhere, Category = "Custom Reflection Capture", meta = (UIMin = "0", UIMAX = "4"))
		float Brightness = 1.f;

	UPROPERTY(VisibleAnywhere, Category = "Custom Reflection Capture")
		UTextureRenderTargetCube* RenderTarget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Custom Reflection Capture")
		UTextureCube* StaticTexture = nullptr;

	UPROPERTY()
		FTimerHandle DeferredUpdateTimer;

	UPROPERTY()
		FRotator PlacedRotation;

	UPROPERTY()
		bool bRemovedUnnecessaryComponents = false;

	//// Functions ////	
public:
	// Sets default values for this actor's properties
	ARotatingBoxReflectionCapture();

#if WITH_EDITOR
	UFUNCTION(CallInEditor, Category = "Custom Reflection Capture")
		void UpdateCapture();

	virtual void OnConstruction(const FTransform& Transform) override;
#endif

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
