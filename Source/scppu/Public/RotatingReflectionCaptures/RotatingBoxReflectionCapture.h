

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RotatingBoxReflectionCapture.generated.h"

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

#if WITH_EDITOR
	UPROPERTY()
		UBillboardComponent* Sprite;

	UPROPERTY()
		UBoxComponent* Box;
#endif

	UPROPERTY()
		UBoxReflectionCaptureComponent* RefCap0;

	UPROPERTY()
		UBoxReflectionCaptureComponent* RefCap90;

	UPROPERTY()
		UBoxReflectionCaptureComponent* RefCap180;

	UPROPERTY()
		UBoxReflectionCaptureComponent* RefCap270;

#if WITH_EDITOR
	UPROPERTY()
		USceneCaptureComponentCube* SceneCaptureCube;
#endif

	UPROPERTY()
		FRotator PlacedRotation;

#if WITH_EDITOR
	UPROPERTY()
		FDateTime LastUpdate;
#endif

	UPROPERTY(VisibleAnywhere, Category = "Custom Reflection Capture")
		UTextureRenderTargetCube* RenderTarget = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "Custom Reflection Capture")
		UTextureCube* StaticTexture = nullptr;

	//// Functions ////	
public:
	// Sets default values for this actor's properties
	ARotatingBoxReflectionCapture();

#if WITH_EDITOR
	UFUNCTION(CallInEditor, Category = "Custom Reflection Capture")
		void UpdateCapture();
#endif

	virtual void OnConstruction(const FTransform& Transform) override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
