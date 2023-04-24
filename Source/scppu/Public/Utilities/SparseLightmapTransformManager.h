

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PrecomputedLightVolume.h"
#include "SparseLightmapTransformManager.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSparseLightmapManager, Log, All);

UCLASS()
class SCPPU_API USparseLightmapTransformManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void OnLevelAddedToWorldCallback(ULevel* Level, UWorld* World);
	void OnLevelRemovedFromWorldCallback(ULevel* Level, UWorld* World);

private:
	FTransform FindLevelTransform(ULevel* Level);
	FPrecomputedLightVolumeData* CopyAndTransformLightVolumeData(FPrecomputedLightVolumeData* Data, FTransform Transform);
	void DeleteLightVolumeData(FPrecomputedLightVolumeData* Data);

	TMap<ULevel*, FPrecomputedLightVolumeData*> LightVolumeDataByLevel;

	// Type punned version of FPrecomputedLightVolumeData
	class FPrecomputedLightVolumeDataExposed
	{
	public:

		FPrecomputedLightVolumeDataExposed() { return; }
		~FPrecomputedLightVolumeDataExposed() { return; }
		
		bool bInitialized;
		FBox Bounds;

		/** Octree containing lighting samples to be used with high quality lightmaps. */
		FLightVolumeOctree HighQualityLightmapOctree;

		/** Octree containing lighting samples to be used with low quality lightmaps. */
		FLightVolumeOctree LowQualityLightmapOctree;

	};

	static_assert(sizeof(FPrecomputedLightVolumeDataExposed) == sizeof(FPrecomputedLightVolumeData), "Memory layout of FPrecomputedLightVolumeDataExposed must exactly match FPrecomputedLightVolumeData");
};
