


#include "Utilities/PerformanceDataGrabber.h"
#include "HardwareInfo.h"
#include "HAL/PlatformMemory.h"
#include "HAL/PlatformMisc.h"
#include "GenericPlatform/GenericPlatformDriver.h"

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
	Parent->NumDrawCalls = GNumDrawCallsRHI[0];
	Parent->NumPrimitivesDrawn = GNumPrimitivesDrawnRHI[0];
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

UPerformanceDataGrabber::UPerformanceDataGrabber()
{
	FString OSVer;
	FString OSVerSub;
	FPlatformMisc::GetOSVersions(OSVer, OSVerSub);
	this->OSVersionLabel = OSVer.TrimStartAndEnd();
	this->OSVersionID = FPlatformMisc::GetOSVersion().TrimStartAndEnd();
	this->CPUInformation.Name = FPlatformMisc::GetCPUBrand().TrimStartAndEnd();
	this->CPUInformation.NumCores = FPlatformMisc::NumberOfCores();
	this->CPUInformation.NumThreads = FPlatformMisc::NumberOfCoresIncludingHyperthreads();
	
	this->GPUInformation.Name = GRHIAdapterName.TrimStartAndEnd();
	this->GPUInformation.DriverVersion = GRHIAdapterUserDriverVersion.TrimStartAndEnd();
	this->GPUInformation.RHIName = FHardwareInfo::GetHardwareInfo(NAME_RHI).TrimStartAndEnd();
	
	FTextureMemoryStats TexMemStats;
	RHIGetTextureMemoryStats(TexMemStats);
	this->GPUInformation.DedicatedVideoMemory = TexMemStats.DedicatedVideoMemory / (1024 * 1024);
	this->TotalPhysicalMemory = FPlatformMemory::GetConstants().TotalPhysical / (1024 * 1024);

	uint64 DiskTotalSpace;
	uint64 DiskFreeSpace;

#if PLATFORM_WINDOWS
	FPlatformMisc::GetDiskTotalAndFreeSpace("C:/", DiskTotalSpace, DiskFreeSpace);
#elif PLATFORM_LINUX
	FPlatformMisc::GetDiskTotalAndFreeSpace("/sys/", DiskTotalSpace, DiskFreeSpace);
#elif PLATFORM_MAC
	// idk about that one
	FPlatformMisc::GetDiskTotalAndFreeSpace("/sys/", DiskTotalSpace, DiskFreeSpace);
#endif
	this->SystemDriveFreeSpace = DiskFreeSpace / (1024 * 1024);

	FPlatformMisc::GetDiskTotalAndFreeSpace(FPaths::ProjectDir(), DiskTotalSpace, DiskFreeSpace);
	this->GameDriveFreeSpace = DiskFreeSpace / (1024 * 1024);
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
