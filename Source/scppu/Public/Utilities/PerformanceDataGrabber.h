

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ChartCreation.h"
#include "PerformanceDataGrabber.generated.h"

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPerformanceDataGrabberDataUpdated);

UENUM(BlueprintType)
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

UENUM(BlueprintType)
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

USTRUCT(BlueprintType)
struct FCPUInformation
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
		FString Name;

	UPROPERTY(BlueprintReadOnly)
		int NumCores;

	UPROPERTY(BlueprintReadOnly)
		int NumThreads;
};

USTRUCT(BlueprintType)
struct FGPUInformation
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
		FString Name;

	UPROPERTY(BlueprintReadOnly)
		FString DriverVersion;

	UPROPERTY(BlueprintReadOnly)
		FString RHIName;

	// In MiB
	UPROPERTY(BlueprintReadOnly)
		int DedicatedVideoMemory;
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

	UPROPERTY(BlueprintAssignable, Category = Default)
		FPerformanceDataGrabberDataUpdated OnFrameDataUpdated;

	UPROPERTY(BlueprintReadOnly, Category = Default)
		bool bIsCapturing = false;

	// Initialized in constructor //

	UPROPERTY(BlueprintReadOnly, Category = ConstantData)
		FString OSVersionLabel;

	UPROPERTY(BlueprintReadOnly, Category = ConstantData)
		FString OSVersionID;

	UPROPERTY(BlueprintReadOnly, Category = ConstantData)
		FCPUInformation CPUInformation;

	UPROPERTY(BlueprintReadOnly, Category = ConstantData)
		FGPUInformation GPUInformation;

	// In MiB
	UPROPERTY(BlueprintReadOnly, Category = ConstantData)
		int32 TotalPhysicalMemory;

	// In MiB
	UPROPERTY(BlueprintReadOnly, Category = ConstantData)
		int32 SystemDriveFreeSpace;

	// In MiB
	UPROPERTY(BlueprintReadOnly, Category = ConstantData)
		int32 GameDriveFreeSpace;

	// Updated per frame //

	UPROPERTY(BlueprintReadOnly, Category = FrameData)
		int FrameIndex;

	UPROPERTY(BlueprintReadOnly, Category = FrameData)
		float DeltaSeconds;

	UPROPERTY(BlueprintReadOnly, Category = FrameData)
		float GameThreadTimeSeconds;

	UPROPERTY(BlueprintReadOnly, Category = FrameData)
		float RenderThreadTimeSeconds;

	UPROPERTY(BlueprintReadOnly, Category = FrameData)
		float RHIThreadTimeSeconds;

	UPROPERTY(BlueprintReadOnly, Category = FrameData)
		float GPUTimeSeconds;

	UPROPERTY(BlueprintReadOnly, Category = FrameData)
		int NumDrawCalls;

	UPROPERTY(BlueprintReadOnly, Category = FrameData)
		int NumPrimitivesDrawn;

	UPROPERTY(BlueprintReadOnly, Category = FrameData)
		float DynamicResolutionScreenPercentage;

	UPROPERTY(BlueprintReadOnly, Category = FrameData)
		float FlushAsyncLoadingTime;

	UPROPERTY(BlueprintReadOnly, Category = FrameData)
		int FlushAsyncLoadingCount;

	UPROPERTY(BlueprintReadOnly, Category = FrameData)
		int SyncLoadCount;

	UPROPERTY(BlueprintReadOnly, Category = FrameData)
		EFrameHitchTypeBlueprintExposed HitchStatus;

	UPROPERTY(BlueprintReadOnly, Category = FrameData)
		EFrameBoundThread FrameBound;

	//// Functions ////	
public:
	UPerformanceDataGrabber();

	UFUNCTION(BlueprintCallable)
		void StartCapture();

	UFUNCTION(BlueprintCallable)
		void StopCapture();

protected:
	virtual void BeginDestroy() override;
};
