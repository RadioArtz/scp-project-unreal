

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ChartCreation.h"
#include "PerformanceDataGrabber.generated.h"

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPerformanceDataGrabberDataUpdated);

UENUM()
enum class EFrameHitchTypeBlueprintExposed : uint8
{
	// We didn't hitch
	NoHitch,

	// We hitched but couldn't isolate which unit caused it
	UnknownUnit,

	// Hitched and it was likely caused by the game thread
	GameThread,

	// Hitched and it was likely caused by the render thread
	RenderThread,

	// Hitched and it was likely caused by the RHI thread
	RHIThread,

	// Hitched and it was likely caused by the GPU
	GPU
};

UENUM()
enum class EFrameBoundThread : uint8
{
	// We are not bound
	None,

	// We are bound by the game thread
	GameThread,

	// We are bound by the render thread
	RenderThread,

	// We are bound by the RHI thread
	RHIThread,

	// We are bound by the GPU 
	GPU
};

class SCPPU_API PerformanceDataGrabberInternal : public IPerformanceDataConsumer
{

protected:
	UPerformanceDataGrabber* Parent;

public:
	PerformanceDataGrabberInternal(UPerformanceDataGrabber* Parent);
	~PerformanceDataGrabberInternal();
	virtual void StartCharting() override;
	virtual void ProcessFrame(const FFrameData& FrameData) override;
	virtual void StopCharting() override;
};

UCLASS(BlueprintType)
class SCPPU_API UPerformanceDataGrabber : public UObject
{
	GENERATED_BODY()

	//// Properties ////
public:

	TSharedPtr<PerformanceDataGrabberInternal> Grabber;

	UPROPERTY(BlueprintReadOnly)
		bool bIsCapturing = false;

	UPROPERTY(BlueprintAssignable)
		FPerformanceDataGrabberDataUpdated OnFrameDataUpdated;

	UPROPERTY(BlueprintReadOnly)
		int FrameIndex;

	UPROPERTY(BlueprintReadOnly)
		float DeltaSeconds;

	UPROPERTY(BlueprintReadOnly)
		float GameThreadTimeSeconds;

	UPROPERTY(BlueprintReadOnly)
		float RenderThreadTimeSeconds;

	UPROPERTY(BlueprintReadOnly)
		float RHIThreadTimeSeconds;

	UPROPERTY(BlueprintReadOnly)
		float GPUTimeSeconds;

	UPROPERTY(BlueprintReadOnly)
		float DynamicResolutionScreenPercentage;

	UPROPERTY(BlueprintReadOnly)
		float FlushAsyncLoadingTime;

	UPROPERTY(BlueprintReadOnly)
		int FlushAsyncLoadingCount;

	UPROPERTY(BlueprintReadOnly)
		int SyncLoadCount;

	UPROPERTY(BlueprintReadOnly)
		EFrameHitchTypeBlueprintExposed HitchStatus;

	UPROPERTY(BlueprintReadOnly)
		EFrameBoundThread FrameBound;

	//// Functions ////	
public:
	UFUNCTION(BlueprintCallable)
		void StartCapture();

	UFUNCTION(BlueprintCallable)
		void StopCapture();

protected:
	virtual void BeginDestroy() override;
};
