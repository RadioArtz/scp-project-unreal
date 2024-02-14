
#include "SparseLightmapConversionSubsystem.h"

#include "Utilities/ExtendedWorldSettings.h"
#include "Utilities/VolumetricLightmapDataAccessHelper.h"
#include "PrecomputedVolumetricLightmap.h"
#include "PrecomputedLightVolume.h"
#include "Editor.h"
#include "GPULightmassSettings.h"
#include "Engine/LevelBounds.h" 

DEFINE_LOG_CATEGORY(LogSparseLightmapConversionSubsystem);

bool USparseLightmapConversionSubsystem::DoesSupportWorldType(EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Editor;
}

bool USparseLightmapConversionSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}

	if (!GEditor->WorldSettingsClass->IsChildOf(AExtendedWorldSettings::StaticClass()))
	{
		return false;
	}

	return true;
}

void USparseLightmapConversionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	UE_LOG(LogSparseLightmapConversionSubsystem, Log, TEXT("%s: Initialized!"), *this->GetName());
}

void USparseLightmapConversionSubsystem::Tick(float DeltaTime)
{
	if (!this->bConversionScheduled && this->IsAnyLightmassBuildActive())
	{
		this->OnLightBuildStarted();
		FlushRenderingCommands();
	}
	else if (this->bConversionScheduled && !this->IsAnyLightmassBuildActive())
	{
		FlushRenderingCommands();
		this->OnLightBuildCompleted();
	}
}

void USparseLightmapConversionSubsystem::Deinitialize()
{
	Super::Deinitialize();
	UE_LOG(LogSparseLightmapConversionSubsystem, Log, TEXT("%s: Deinitialized!"), *this->GetName());
}

void USparseLightmapConversionSubsystem::OnLightBuildStarted()
{
	AExtendedWorldSettings* WorldSettings = this->GetActiveWorldSettingsChecked();

	if (WorldSettings->LightmassSettings.VolumeLightingMethod == VLM_SparseVolumeLightingSamples && WorldSettings->bUseVolumetricLightmapAlgorithmForSparseLightmap)
	{
		this->bConversionScheduled = true;
		WorldSettings->LightmassSettings.VolumeLightingMethod = VLM_VolumetricLightmap;
		this->GetWorld()->Scene->UpdateSceneSettings(WorldSettings);
		UE_LOG(LogSparseLightmapConversionSubsystem, Log, TEXT("%s: Light build started, scheduled lightmap conversion"), *this->GetName());
	}
}

void USparseLightmapConversionSubsystem::OnLightBuildCompleted()
{
	AExtendedWorldSettings* WorldSettings = this->GetActiveWorldSettingsChecked();
	check(WorldSettings->LightmassSettings.VolumeLightingMethod == VLM_VolumetricLightmap)
	this->bConversionScheduled = false;
	WorldSettings->LightmassSettings.VolumeLightingMethod = VLM_SparseVolumeLightingSamples;
	this->GetWorld()->Scene->UpdateSceneSettings(WorldSettings);

	UMapBuildDataRegistry* MapRegistry = this->GetActivePersistentLevel()->MapBuildData;
	FGuid LevelBuildDataId = this->GetActivePersistentLevel()->LevelBuildDataId;
	if (MapRegistry == nullptr)
	{
		UE_LOG(LogSparseLightmapConversionSubsystem, Error, TEXT("%s: No map registry found, aborting lightmap conversion..."), *this->GetName());
		return;
	}

	FPrecomputedVolumetricLightmapData* VolumetricLightmapData = MapRegistry->GetLevelPrecomputedVolumetricLightmapBuildData(LevelBuildDataId);
	if (VolumetricLightmapData == nullptr)
	{
		UE_LOG(LogSparseLightmapConversionSubsystem, Error, TEXT("%s: No volumetric lightmap data found, aborting lightmap conversion..."), *this->GetName());
		return;
	}

	this->GetActivePersistentLevel()->PrecomputedVolumetricLightmap->RemoveFromScene(this->GetWorld()->Scene);
	this->GetActivePersistentLevel()->PrecomputedLightVolume->RemoveFromScene(this->GetWorld()->Scene);

	FPrecomputedLightVolumeData* LightVolumeData = this->CreateLightVolumeDataFromVolumetricLightmapData(VolumetricLightmapData);
	
	this->ClearMapRegistryPrecomputedLightmapEntries(MapRegistry, LevelBuildDataId);
	VolumetricLightmapData = nullptr; // gets deleted by the function above!!

	MapRegistry->AddLevelPrecomputedLightVolumeBuildData(LevelBuildDataId, LightVolumeData);
	this->GetActivePersistentLevel()->PrecomputedLightVolume->AddToScene(this->GetWorld()->Scene, MapRegistry, LevelBuildDataId);
	UE_LOG(LogSparseLightmapConversionSubsystem, Log, TEXT("%s: Lightmap conversion completed"), *this->GetName());
}

bool USparseLightmapConversionSubsystem::IsAnyLightmassBuildActive()
{
	bool bAnyLightmassActive = false;
	check(GEditor);
	if (IsValid(GEditor))
	{
		bAnyLightmassActive = bAnyLightmassActive || GEditor->IsLightingBuildCurrentlyRunning() || GEditor->IsLightingBuildCurrentlyExporting();
	}

	UGPULightmassSubsystem* GPULightmassSubsystem = this->GetWorld()->GetSubsystem<UGPULightmassSubsystem>();
	if (IsValid(GPULightmassSubsystem))
	{
		bAnyLightmassActive = bAnyLightmassActive || GPULightmassSubsystem->IsRunning();
	}

	return bAnyLightmassActive;
}

ULevel* USparseLightmapConversionSubsystem::GetActivePersistentLevel()
{
	return this->GetWorld()->PersistentLevel;
}

AExtendedWorldSettings* USparseLightmapConversionSubsystem::GetActiveWorldSettingsChecked()
{
	AExtendedWorldSettings* WorldSettings = (AExtendedWorldSettings*)this->GetActivePersistentLevel()->GetWorldSettings();
	check(WorldSettings);
	return WorldSettings;
}

void USparseLightmapConversionSubsystem::ClearMapRegistryPrecomputedLightmapEntries(UMapBuildDataRegistry* MapRegistry, const FGuid LevelBuildDataId)
{
	check(MapRegistry)
	FPrecomputedVolumetricLightmapData* VolumetricLightmapData = MapRegistry->GetLevelPrecomputedVolumetricLightmapBuildData(LevelBuildDataId);
	FPrecomputedLightVolumeData* LightVolumeData = MapRegistry->GetLevelPrecomputedLightVolumeBuildData(LevelBuildDataId);

	// This operation is considerd undefined behavior (violating the strict aliasing rule).
	// We do this to read otherwise private members of UMapBuildDataRegistry to be able to properly clear old light volumes without having to actually fully rebuild the map registry.
	// The use of memcpy leads to failed assertions further down the call line. We may check it out later again.
	// For now, it seems to do fine.
	UPunnedMapBuildDataRegistry* MapRegistryPunned = reinterpret_cast<UPunnedMapBuildDataRegistry*>(MapRegistry);
	if (VolumetricLightmapData != nullptr)
	{
		MapRegistryPunned->LevelPrecomputedVolumetricLightmapBuildData.Remove(LevelBuildDataId);
		BeginReleaseResource(VolumetricLightmapData);
	}

	if (LightVolumeData != nullptr)
	{
		MapRegistryPunned->LevelPrecomputedLightVolumeBuildData.Remove(LevelBuildDataId);
	}

	FlushRenderingCommands(); // Make sure the render thread is not using the data anymore
	if (VolumetricLightmapData != nullptr)
	{
		delete VolumetricLightmapData;
		VolumetricLightmapData = nullptr;
	}

	if (LightVolumeData != nullptr)
	{
		this->DeleteLightVolumeData(LightVolumeData);
		LightVolumeData = nullptr;
	}

	MapRegistry->MarkPackageDirty();
}

bool USparseLightmapConversionSubsystem::IsPointIntersectingStaticGeometry(FVector Point)
{
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionParams;
	CollisionParams.bTraceComplex = true;
	CollisionParams.bFindInitialOverlaps = true;
	CollisionParams.TraceTag = "SparseLightmapConversionGeometry";
	this->GetWorld()->OverlapMultiByChannel(OverlapResults, Point, FQuat::Identity, ECollisionChannel::ECC_Visibility, FCollisionShape::MakeSphere(20.f), CollisionParams);

	for (FOverlapResult Result : OverlapResults)
	{
		if (Result.Actor->GetRootComponent()->Mobility == EComponentMobility::Static)
		{
			return true;
		}
	}

	return false;
}

bool USparseLightmapConversionSubsystem::IsPointAboveFloor(FVector Point)
{
	AExtendedWorldSettings* WorldSettings = this->GetActiveWorldSettingsChecked();
	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams CollisionParams;
	CollisionParams.bTraceComplex = true;
	CollisionParams.TraceTag = "SparseLightmapConversionFloor";
	const float HalfMaxHeightAboveFloor = WorldSettings->SparseLightmapConversionSettings.MaxHeightAboveFloor / 2.f;
	this->GetWorld()->OverlapMultiByChannel(OverlapResults, Point - FVector(0, 0, HalfMaxHeightAboveFloor), FQuat::Identity, ECollisionChannel::ECC_Visibility, FCollisionShape::MakeCapsule(1.f, HalfMaxHeightAboveFloor), CollisionParams);

	for (FOverlapResult Result : OverlapResults)
	{
		if (Result.Actor->GetRootComponent()->Mobility == EComponentMobility::Static)
		{
			return true;
		}
	}

	return false;
}

FPrecomputedLightVolumeData* USparseLightmapConversionSubsystem::CreateLightVolumeDataFromVolumetricLightmapData(FPrecomputedVolumetricLightmapData* VolumetricLightmapData)
{
	check(VolumetricLightmapData)
	AExtendedWorldSettings* WorldSettings = this->GetActiveWorldSettingsChecked();
	FPrecomputedLightVolumeData* LightVolumeData = new FPrecomputedLightVolumeData();

	FBox LightVolumeBounds;
	if (WorldSettings->SparseLightmapConversionSettings.bOverrideLightmapBounds)
	{
		LightVolumeBounds = WorldSettings->SparseLightmapConversionSettings.LightmapBounds;
	}
	else
	{
		LightVolumeBounds = ALevelBounds::CalculateLevelBounds(this->GetActivePersistentLevel());
	}

	LightVolumeData->Initialize(LightVolumeBounds.ShiftBy(WorldSettings->SparseLightmapConversionSettings.SampleOffset));
	FVolumetricLightmapDataAccessHelper AccessHelper = FVolumetricLightmapDataAccessHelper(VolumetricLightmapData);

	const float SampleStepSize = WorldSettings->SparseLightmapConversionSettings.SampleInterval;
	for (float x = LightVolumeBounds.Min.X; x < LightVolumeBounds.Max.X; x += SampleStepSize)
	{
		for (float y = LightVolumeBounds.Min.Y; y < LightVolumeBounds.Max.Y; y += SampleStepSize)
		{
			for (float z = LightVolumeBounds.Min.Z; z < LightVolumeBounds.Max.Z; z += SampleStepSize)
			{
				const FVector Position = FVector(x, y, z) + WorldSettings->SparseLightmapConversionSettings.SampleOffset;
				if (!VolumetricLightmapData->GetBounds().IsInsideOrOn(Position))
				{
					continue;
				}

				if (WorldSettings->SparseLightmapConversionSettings.bOnlySamplePointsOutsideGeometry && this->IsPointIntersectingStaticGeometry(Position))
				{
					continue;
				}

				if (WorldSettings->SparseLightmapConversionSettings.bOnlySamplePointsAboveFloor && !this->IsPointAboveFloor(Position))
				{
					continue;
				}

				FVolumeLightingSample NewSample;
				NewSample.Position = Position;
				NewSample.Radius = SampleStepSize;
				NewSample.Lighting = AccessHelper.GetLightColor(Position);
				NewSample.DirectionalLightShadowing = AccessHelper.GetDirectionalLightShadowing(Position);
				NewSample.SetPackedSkyBentNormal(AccessHelper.GetSkyBentNormal(Position));
				LightVolumeData->AddHighQualityLightingSample(NewSample);
				LightVolumeData->AddLowQualityLightingSample(NewSample);
			}
		}
	}

	LightVolumeData->FinalizeSamples();
	return LightVolumeData;
}

void USparseLightmapConversionSubsystem::DeleteLightVolumeData(FPrecomputedLightVolumeData* Data)
{
	check(Data);

	// The deconstructor of FPrecomutedLightVolumeData is not exported, so we have to it this way
	Data->InvalidateLightingCache();
	operator delete(Data);
}
