

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

DECLARE_LOG_CATEGORY_EXTERN(LogRotatingReflectionCapture, Log, All);

UCLASS(hidecategories = (Collision, Attachment, Actor))
class SCPPU_API ARotatingBoxReflectionCapture : public AActor
{
	GENERATED_BODY()
	
	//// Properties ////
public:
	UPROPERTY()
		USceneComponent* Root;

	UPROPERTY()
		UBillboardComponent* EditorSprite;

	UPROPERTY()
		UBoxComponent* EditorBox;

	UPROPERTY()
		UBillboardComponent* CaptureOffsetSprite;

	UPROPERTY()
		UBoxReflectionCaptureComponent* ReflectionCapture0Deg;

	UPROPERTY()
		UBoxReflectionCaptureComponent* ReflectionCapture90Deg;

	UPROPERTY()
		UBoxReflectionCaptureComponent* ReflectionCapture180Deg;

	UPROPERTY()
		UBoxReflectionCaptureComponent* ReflectionCapture270Deg;

	UPROPERTY()
		USceneCaptureComponentCube* SceneCaptureCube;

	// Number of recaptures to capture is own reflection
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "CustomReflectionCapture", meta = (ClampMin = 0, ClampMax = 10))
		int NumReflectiveCaputures = 2;

	// Brighness of the capture
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomReflectionCapture", meta = (UIMin = 0, UIMAX = 4))
		float Brightness = 1.f;

	// World space offset to apply before capturing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CustomReflectionCapture")
		FVector CaptureOffset;

	// Last element is applied
	UPROPERTY(VisibleAnywhere, Category = "Debug")
		TArray<UTextureRenderTargetCube*> RenderTargets;

	// Last element is applied
	UPROPERTY(VisibleAnywhere, Category = "Debug")
		TArray<UTextureCube*> StaticTextures;

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
	UFUNCTION(CallInEditor, Category = "CustomReflectionCapture")
		void UpdateCapture();

	virtual void OnConstruction(const FTransform& Transform) override;
#endif

	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
