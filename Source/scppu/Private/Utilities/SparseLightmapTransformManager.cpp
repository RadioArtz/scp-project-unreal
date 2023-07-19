


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
	Super::Initialize(Collection);
	FWorldDelegates::LevelAddedToWorld.AddUObject(this, &USparseLightmapTransformManager::OnLevelAddedToWorldCallback);
	FWorldDelegates::LevelRemovedFromWorld.AddUObject(this, &USparseLightmapTransformManager::OnLevelRemovedFromWorldCallback);
	UE_LOG(LogSparseLightmapManager, Log, TEXT("%s: Initialized!"), *this->GetName());
}

void USparseLightmapTransformManager::Tick(float DeltaTime)
{
	if (this->HangingLightVolumeData.Num() > this->NumMaxAllowedHangingLightVolumeData)
	{
		this->ClearHangingLightVolumeData();
	}
}

void USparseLightmapTransformManager::Deinitialize()
{
	if (LightVolumeDataByLevel.Num() > 0)
	{
		UE_LOG(LogSparseLightmapManager, Warning, TEXT("%s: Forcefully marking transformed lightmaps of %i levels as hanging and ready for deletion"), *this->GetName(), LightVolumeDataByLevel.Num());

		TMap<ULevel*, FPrecomputedLightVolumeData*> LightVolumeDataByLevelCopy = LightVolumeDataByLevel;
		for (auto Kvp : LightVolumeDataByLevelCopy)
		{
			this->OnLevelRemovedFromWorldCallback(Kvp.Key, this->GetWorld());
		}
	}

	this->ClearHangingLightVolumeData();
	Super::Deinitialize();
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
	this->LightVolumeDataByLevel.RemoveAndCopyValue(Level, TransformedVolumeData);
	this->HangingLightVolumeData.Add(TransformedVolumeData);
	UE_LOG(LogSparseLightmapManager, Log, TEXT("%s: Marked transformed lightmap of level '%s' as hanging and ready for deletion"), *this->GetName(), *Level->GetPathName());
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

	// This operation is considerd undefined behavior (violating strict type aliasing).
	// We do this to read otherwise private variables of FPrecomputedLightVolumeData to be able to properly copy and transform the light samples for transformed levels.
	// The use of memcpy leads to assertions further down the call line, We may check it out later again.
	// For now, it seems to do fine.
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

void USparseLightmapTransformManager::ClearHangingLightVolumeData()
{
	if (this->HangingLightVolumeData.Num() <= 0)
	{
		return;
	}

	FlushRenderingCommands(); // We don't want the render thread to still use the data while we delete it.
	int NumHangingLightVolumeDataRemoved = 0;
	while (this->HangingLightVolumeData.Num() > 0)
	{
		FPrecomputedLightVolumeData* TransformedVolumeData = HangingLightVolumeData[0];
		HangingLightVolumeData.RemoveAt(0);
		this->DeleteLightVolumeData(TransformedVolumeData);
		NumHangingLightVolumeDataRemoved++;
	}

	UE_LOG(LogSparseLightmapManager, Log, TEXT("%s: Successfully cleaned up %i hanging transformed lightmaps"), *this->GetName(), NumHangingLightVolumeDataRemoved);
}

void USparseLightmapTransformManager::DeleteLightVolumeData(FPrecomputedLightVolumeData* Data)
{
	check(Data != nullptr);

	// The deconstructor of FPrecomutedLightVolumeData is not exported, so we have to it this way
	Data->InvalidateLightingCache();
	operator delete(Data);
}
