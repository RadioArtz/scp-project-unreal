


#include "Utilities/SparseLightmapTransformManager.h"
#include "Engine/Level.h"
#include "Engine/MapBuildDataRegistry.h"
#include "Engine/LevelStreaming.h"

DEFINE_LOG_CATEGORY(LogSparseLightmapManager);

bool USparseLightmapTransformManager::DoesSupportWorldType(EWorldType::Type WorldType) const
{
	TSet<EWorldType::Type> SupportedWorlds;
	SupportedWorlds.Append({ EWorldType::Game, EWorldType::PIE, EWorldType::GamePreview });
	return SupportedWorlds.Contains(WorldType);
}

void USparseLightmapTransformManager::Initialize(FSubsystemCollectionBase& Collection)
{
	FWorldDelegates::LevelAddedToWorld.AddUObject(this, &USparseLightmapTransformManager::OnLevelAddedToWorldCallback);
	FWorldDelegates::LevelRemovedFromWorld.AddUObject(this, &USparseLightmapTransformManager::OnLevelRemovedFromWorldCallback);
	UE_LOG(LogSparseLightmapManager, Log, TEXT("%s: Initialized!"), *this->GetName());
}

void USparseLightmapTransformManager::Deinitialize()
{
	if (LightVolumeDataByLevel.Num() > 0)
	{
		UE_LOG(LogSparseLightmapManager, Warning, TEXT("%s: Forcefully cleaning up transformed lightmaps of %i levels"), *this->GetName(), LightVolumeDataByLevel.Num());

		TMap<ULevel*, FPrecomputedLightVolumeData*> LightVolumeDataByLevelCopy = LightVolumeDataByLevel;
		for (auto Kvp : LightVolumeDataByLevelCopy)
		{
			this->OnLevelRemovedFromWorldCallback(Kvp.Key, this->GetWorld());
		}
	}

	UE_LOG(LogSparseLightmapManager, Log, TEXT("%s: Deinitialized!"), *this->GetName());
}

void USparseLightmapTransformManager::OnLevelAddedToWorldCallback(ULevel* Level, UWorld* World)
{
	if (this->GetWorld() != World)
	{
		UE_LOG(LogSparseLightmapManager, Verbose, TEXT("%s: Level '%s' was not added to our world and will be ignored"), *this->GetName(), *Level->GetPathName());
		return;
	}

	if (this->GetWorld()->PersistentLevel == Level)
	{
		UE_LOG(LogSparseLightmapManager, Verbose, TEXT("%s: Level '%s' is the persistent level and will be ignored"), *this->GetName(), *Level->GetPathName());
		return;
	}

	if (Level->MapBuildData == nullptr)
	{
		UE_LOG(LogSparseLightmapManager, Verbose, TEXT("%s: Level '%s' has no build data and will be ignored"), *this->GetName(), *Level->GetPathName());
		return;
	}

	FTransform LevelTransform = this->FindLevelTransform(Level);
	if (LevelTransform.Equals(FTransform::Identity))
	{
		UE_LOG(LogSparseLightmapManager, Verbose, TEXT("%s: Level '%s' is not transformed and will be ignored"), *this->GetName(), *Level->GetPathName());
		return;
	}

	FPrecomputedLightVolume* Volume = Level->PrecomputedLightVolume;
	if (!Volume->IsAddedToScene())
	{
		UE_LOG(LogSparseLightmapManager, Verbose, TEXT("%s: Level '%s' has no light volume added to the scene and will be ignored"), *this->GetName(), *Level->GetPathName());
		return;
	}
		
	FPrecomputedLightVolumeData* OriginalVolumeData = Level->MapBuildData->GetLevelPrecomputedLightVolumeBuildData(Level->LevelBuildDataId);
	FPrecomputedLightVolumeData* TransformedVolumeData = this->CopyAndTransformLightVolumeData(OriginalVolumeData, LevelTransform);
	LightVolumeDataByLevel.Add(Level, TransformedVolumeData);

	Volume->RemoveFromScene(this->GetWorld()->Scene);
	Level->MapBuildData->AddLevelPrecomputedLightVolumeBuildData(Level->LevelBuildDataId, TransformedVolumeData);
	Volume->AddToScene(this->GetWorld()->Scene, Level->MapBuildData, Level->LevelBuildDataId);
	Level->MapBuildData->AddLevelPrecomputedLightVolumeBuildData(Level->LevelBuildDataId, OriginalVolumeData);
	UE_LOG(LogSparseLightmapManager, Log, TEXT("%s: Successfully transformed lightmap of level '%s' to: %s"), *this->GetName(), *Level->GetPathName(), *LevelTransform.ToString());
}

void USparseLightmapTransformManager::OnLevelRemovedFromWorldCallback(ULevel* Level, UWorld* World)
{
	if (this->GetWorld() != World)
	{
		UE_LOG(LogSparseLightmapManager, Verbose, TEXT("%s: Level '%s' was not removed from our world and will be ignored"), *this->GetName(), *Level->GetPathName());
		return;
	}

	if (!this->LightVolumeDataByLevel.Contains(Level))
	{
		UE_LOG(LogSparseLightmapManager, Verbose, TEXT("%s: Level '%s' was not touched by us and will be ignored"), *this->GetName(), *Level->GetPathName());
		return;
	}

	check(!Level->PrecomputedLightVolume->IsAddedToScene());

	FPrecomputedLightVolumeData* TransformedVolumeData;
	LightVolumeDataByLevel.RemoveAndCopyValue(Level, TransformedVolumeData);
	this->DeleteLightVolumeData(TransformedVolumeData);
	UE_LOG(LogSparseLightmapManager, Log, TEXT("%s: Successfully cleaned up transformed lightmap of level '%s'"), *this->GetName(), *Level->GetPathName());
}

FTransform USparseLightmapTransformManager::FindLevelTransform(ULevel* Level)
{
	for (auto StreamingLevel : this->GetWorld()->GetStreamingLevels())
	{
		if (StreamingLevel->GetLoadedLevel() == Level)
		{
			return StreamingLevel->LevelTransform;
		}
	}

	return FTransform::Identity;
}

FPrecomputedLightVolumeData* USparseLightmapTransformManager::CopyAndTransformLightVolumeData(FPrecomputedLightVolumeData* Data, FTransform Transform)
{
	check(Data != nullptr);

	FPrecomputedLightVolumeData* TransformedData = new FPrecomputedLightVolumeData();
	TransformedData->Initialize(Data->GetBounds().TransformBy(Transform.ToMatrixNoScale()));

	// Undefined behavior (type aliasing), but I don't really have a choice here (maybe memcpy it?)
	FPrecomputedLightVolumeDataExposed* DataExposed = reinterpret_cast<FPrecomputedLightVolumeDataExposed*>(Data);

	DataExposed->HighQualityLightmapOctree.FindAllElements([TransformedData, Transform](const FVolumeLightingSample& OriginalSample)
		{
			FVolumeLightingSample NewSample;
			NewSample.DirectionalLightShadowing = OriginalSample.DirectionalLightShadowing;
			NewSample.Lighting = OriginalSample.Lighting;
			NewSample.PackedSkyBentNormal = OriginalSample.PackedSkyBentNormal;
			NewSample.Position = Transform.TransformPosition(OriginalSample.Position);
			NewSample.Radius = OriginalSample.Radius;
			TransformedData->AddHighQualityLightingSample(NewSample);
		});

	DataExposed->LowQualityLightmapOctree.FindAllElements([TransformedData, Transform](const FVolumeLightingSample& OriginalSample)
		{
			FVolumeLightingSample NewSample;
			NewSample.DirectionalLightShadowing = OriginalSample.DirectionalLightShadowing;
			NewSample.Lighting = OriginalSample.Lighting;
			NewSample.PackedSkyBentNormal = OriginalSample.PackedSkyBentNormal;
			NewSample.Position = Transform.TransformPosition(OriginalSample.Position);
			NewSample.Radius = OriginalSample.Radius;
			TransformedData->AddLowQualityLightingSample(NewSample);
		});

	TransformedData->FinalizeSamples();
	return TransformedData;
}

void USparseLightmapTransformManager::DeleteLightVolumeData(FPrecomputedLightVolumeData* Data)
{
	check(Data != nullptr);
	Data->InvalidateLightingCache();
	operator delete(Data);
}
