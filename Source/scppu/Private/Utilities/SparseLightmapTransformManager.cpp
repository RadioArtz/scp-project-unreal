


#include "Utilities/SparseLightmapTransformManager.h"
#include "Engine/Level.h"
#include "Engine/MapBuildDataRegistry.h"
#include "Engine/LevelStreaming.h"

DEFINE_LOG_CATEGORY(LogSparseLightmapManager);

void USparseLightmapTransformManager::Initialize(FSubsystemCollectionBase& Collection)
{
	FWorldDelegates::LevelAddedToWorld.AddUObject(this, &USparseLightmapTransformManager::UpdateLevelSparseLightmap);
	UE_LOG(LogSparseLightmapManager, Log, TEXT("%s: Initialized!"), *this->GetName());
}

bool USparseLightmapTransformManager::DoesSupportWorldType(EWorldType::Type WorldType) const
{
	switch (WorldType)
	{
	case EWorldType::None:
		return false;
	case EWorldType::Game:
		return true;
	case EWorldType::Editor:
		return false;
	case EWorldType::PIE:
		return true;
	case EWorldType::EditorPreview:
		return false;
	case EWorldType::GamePreview:
		return true;
	case EWorldType::GameRPC:
		return false;
	case EWorldType::Inactive:
		return false;
	default:
		return false;
	}
}

void USparseLightmapTransformManager::UpdateLevelSparseLightmap(ULevel* Level, UWorld* World)
{
	FString LevelPathName = Level->GetPathName();

	if (this->GetWorld() != World)
	{
		UE_LOG(LogSparseLightmapManager, Verbose, TEXT("%s: Level '%s' was not added to our world and will be ignored"), *this->GetName(), *LevelPathName);
		return;
	}

	if (this->GetWorld()->PersistentLevel == Level)
	{
		UE_LOG(LogSparseLightmapManager, Verbose, TEXT("%s: Level '%s' is the persistent level and will be ignored"), *this->GetName(), *LevelPathName);
		return;
	}

	if (Level->MapBuildData == nullptr)
	{
		UE_LOG(LogSparseLightmapManager, Verbose, TEXT("%s: Level '%s' has no build data and will be ignored"), *this->GetName(), *LevelPathName);
		return;
	}

	FTransform LevelTransform;
	bool bStreamingLevelFound = false;
	for (auto StreamingLevel : World->GetStreamingLevels())
	{
		if (StreamingLevel->GetLoadedLevel() == Level)
		{
			LevelTransform = StreamingLevel->LevelTransform;
			bStreamingLevelFound = true;
			break;
		}
	}

	if (!bStreamingLevelFound)
	{
		UE_LOG(LogSparseLightmapManager, Verbose, TEXT("%s: Steaming level of level '%s' could not be found and will be ignored"), *this->GetName(), *LevelPathName);
		return;
	}

	FGuid BuildDataId = Level->LevelBuildDataId;
	UMapBuildDataRegistry* BuildDataRegistry = Level->MapBuildData;
	FPrecomputedLightVolume* Volume = Level->PrecomputedLightVolume;
	FPrecomputedLightVolumeData* OriginalLightVolumeData = BuildDataRegistry->GetLevelPrecomputedLightVolumeBuildData(BuildDataId);

	if (!Volume->IsAddedToScene())
	{
		UE_LOG(LogSparseLightmapManager, Error, TEXT("%s: Volume of level '%s' is not added to scene, aborting..."), *this->GetName(), *LevelPathName);
		return;
	}

	if (!OriginalLightVolumeData || !OriginalLightVolumeData->IsInitialized())
	{
		UE_LOG(LogSparseLightmapManager, Error, TEXT("%s: Lightmap data of '%s' is invalid, aborting..."), *this->GetName(), *LevelPathName);
		return;
	}

	FPrecomputedLightVolumeDataExposed* OrginalLightVolumeDataExposed = reinterpret_cast<FPrecomputedLightVolumeDataExposed*>(OriginalLightVolumeData);
	FPrecomputedLightVolumeData* NewLightVolumeData = new FPrecomputedLightVolumeData();
	FSceneInterface* Scene = World->Scene;

	Volume->RemoveFromScene(Scene);
	NewLightVolumeData->Initialize(OriginalLightVolumeData->GetBounds().TransformBy(LevelTransform.ToMatrixNoScale()));

	OrginalLightVolumeDataExposed->HighQualityLightmapOctree.FindAllElements([NewLightVolumeData, LevelTransform](const FVolumeLightingSample& OriginalSample)
		{
			FVolumeLightingSample NewSample = FVolumeLightingSample();
			NewSample.DirectionalLightShadowing = OriginalSample.DirectionalLightShadowing;
			NewSample.Lighting = OriginalSample.Lighting;
			NewSample.PackedSkyBentNormal = OriginalSample.PackedSkyBentNormal;
			NewSample.Position = LevelTransform.TransformPosition(OriginalSample.Position);
			NewSample.Radius = OriginalSample.Radius;
			NewLightVolumeData->AddHighQualityLightingSample(NewSample);
		});

	OrginalLightVolumeDataExposed->LowQualityLightmapOctree.FindAllElements([NewLightVolumeData, LevelTransform](const FVolumeLightingSample& OriginalSample)
		{
			FVolumeLightingSample NewSample = FVolumeLightingSample();
			NewSample.DirectionalLightShadowing = OriginalSample.DirectionalLightShadowing;
			NewSample.Lighting = OriginalSample.Lighting;
			NewSample.PackedSkyBentNormal = OriginalSample.PackedSkyBentNormal;
			NewSample.Position = LevelTransform.TransformPosition(OriginalSample.Position);
			NewSample.Radius = OriginalSample.Radius;
			NewLightVolumeData->AddLowQualityLightingSample(NewSample);
		});

	NewLightVolumeData->FinalizeSamples();
	BuildDataRegistry->AddLevelPrecomputedLightVolumeBuildData(BuildDataId, NewLightVolumeData);
	Volume->AddToScene(Scene, BuildDataRegistry, BuildDataId);
	BuildDataRegistry->AddLevelPrecomputedLightVolumeBuildData(BuildDataId, OriginalLightVolumeData);
	UE_LOG(LogSparseLightmapManager, Log, TEXT("%s: Successfully moved lightmap of level '%s' to: %s"), *this->GetName(), *LevelPathName, *LevelTransform.ToString());
}
