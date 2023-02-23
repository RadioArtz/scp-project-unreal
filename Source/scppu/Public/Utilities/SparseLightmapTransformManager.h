

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
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;
	virtual void UpdateLevelSparseLightmap(ULevel* Level, UWorld* World);

protected:

	// Used to access usally private members of FPrecomputedLightVolumeData via reinterpret_cast
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
};
