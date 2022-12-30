


#include "Utilities/PerformanceDataGrabber.h"

PerformanceDataGrabberInternal::PerformanceDataGrabberInternal(UPerformanceDataGrabber* Parent)
{
	check(Parent != nullptr);
	this->Parent = Parent;
};

PerformanceDataGrabberInternal::~PerformanceDataGrabberInternal()
{
}

void PerformanceDataGrabberInternal::StartCharting()
{
}

void PerformanceDataGrabberInternal::ProcessFrame(const FFrameData& FrameData)
{
	check(Parent != nullptr);

	Parent->FrameIndex = GFrameNumber;
	Parent->DeltaSeconds = FrameData.DeltaSeconds;
	Parent->GameThreadTimeSeconds = FrameData.GameThreadTimeSeconds;
	Parent->RenderThreadTimeSeconds = FrameData.RenderThreadTimeSeconds;
	Parent->RHIThreadTimeSeconds = FrameData.RHIThreadTimeSeconds;
	Parent->GPUTimeSeconds = FrameData.GPUTimeSeconds;
	Parent->DynamicResolutionScreenPercentage = FrameData.DynamicResolutionScreenPercentage;
	Parent->FlushAsyncLoadingTime = FrameData.FlushAsyncLoadingTime;
	Parent->FlushAsyncLoadingCount = FrameData.FlushAsyncLoadingCount;
	Parent->SyncLoadCount = FrameData.SyncLoadCount;
	Parent->HitchStatus = (EFrameHitchTypeBlueprintExposed)(uint8)FrameData.HitchStatus;

	if (FrameData.bGameThreadBound)
	{
		Parent->FrameBound = EFrameBoundThread::GameThread;
	}
	else if (FrameData.bRenderThreadBound)
	{
		Parent->FrameBound = EFrameBoundThread::RenderThread;
	}
	else if (FrameData.bRHIThreadBound)
	{
		Parent->FrameBound = EFrameBoundThread::RHIThread;
	}
	else if (FrameData.bGPUBound)
	{
		Parent->FrameBound = EFrameBoundThread::GPU;
	}
	else
	{
		Parent->FrameBound = EFrameBoundThread::None;
	}

	Parent->OnFrameDataUpdated.Broadcast();
}

void PerformanceDataGrabberInternal::StopCharting()
{
}

void UPerformanceDataGrabber::StartCapture()
{
	if (this->bIsCapturing)
	{
		return;
	}

	this->Grabber = MakeShareable(new PerformanceDataGrabberInternal(this));
	GEngine->AddPerformanceDataConsumer(this->Grabber);
	this->bIsCapturing = true;
}

void UPerformanceDataGrabber::StopCapture()
{
	if (!this->bIsCapturing)
	{
		return;
	}

	GEngine->RemovePerformanceDataConsumer(this->Grabber);
	this->Grabber.Reset();
	this->bIsCapturing = false;
}

void UPerformanceDataGrabber::BeginDestroy()
{
	if (this->bIsCapturing)
	{
		this->StopCapture();
	}

	Super::BeginDestroy();
}
