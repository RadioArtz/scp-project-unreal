

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PrecomputedLightVolume.h"
#include "SparseLightmapTransformSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSparseLightmapTransformSubsystem, Log, All);

class FPrecomputedVolumetricLightmapData;

// Used to properly handle transforming sparse lightmaps of levels added to the world with different transforms
UCLASS(Transient)
class SCPPU_API USparseLightmapTransformSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

	//// Functions ////
public:
	virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual TStatId GetStatId() const override { return this->GetStatID(); }
	virtual void Tick(float DeltaTime) override;
	virtual void Deinitialize() override;

	void OnLevelAddedToWorldCallback(ULevel* Level, UWorld* World);
	void OnLevelRemovedFromWorldCallback(ULevel* Level, UWorld* World);

private:
	FTransform FindLevelTransform(ULevel* Level);
	FPrecomputedLightVolumeData* CopyAndTransformLightVolumeData(FPrecomputedLightVolumeData* Data, FTransform Transform);
	void ClearHangingLightVolumeData();
	void DeleteLightVolumeData(FPrecomputedLightVolumeData* Data);

	//// Properties ////
public:
	int NumMaxAllowedHangingLightVolumeData = 10;

private:
	TMap<ULevel*, FPrecomputedLightVolumeData*> LightVolumeDataByLevel;
	TArray<FPrecomputedLightVolumeData*> HangingLightVolumeData;

	//// Types ////
private:

	// Type punned version of FPrecomputedLightVolumeData. See CopyAndTransformLightVolumeData() for more info.
	class FPunnedPrecomputedLightVolumeData
	{
	public:

		FPunnedPrecomputedLightVolumeData() { return; }
		~FPunnedPrecomputedLightVolumeData() { return; }
		
		bool bInitialized;
		FBox Bounds;

		/** Octree containing lighting samples to be used with high quality lightmaps. */
		FLightVolumeOctree HighQualityLightmapOctree;

		/** Octree containing lighting samples to be used with low quality lightmaps. */
		FLightVolumeOctree LowQualityLightmapOctree;
	};

	// Used as a failsafe to catch bad type puns. See CopyAndTransformLightVolumeData() for more info.
	static_assert(sizeof(FPunnedPrecomputedLightVolumeData) == sizeof(FPrecomputedLightVolumeData), "Memory layout of FPunnedPrecomputedLightVolumeData must exactly match FPrecomputedLightVolumeData");
};
