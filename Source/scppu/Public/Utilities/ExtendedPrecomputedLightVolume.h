

#pragma once

#include "CoreMinimal.h"
#include "PrecomputedLightVolume.h"

// THIS FILE IS TEMPORARY //

class FExposedPrecomputedLightVolumeData
{
public:

	FExposedPrecomputedLightVolumeData();
	~FExposedPrecomputedLightVolumeData();

	friend FArchive& operator<<(FArchive& Ar, FExposedPrecomputedLightVolumeData& Volume);
	friend FArchive& operator<<(FArchive& Ar, FExposedPrecomputedLightVolumeData*& Volume);

	/** Frees any previous samples, prepares the volume to have new samples added. */
	void Initialize(const FBox& NewBounds);

	/** Adds a high quality lighting sample. */
	//void AddHighQualityLightingSample(const FVolumeLightingSample& NewHighQualitySample);

	/** Adds a low quality lighting sample. */
	//void AddLowQualityLightingSample(const FVolumeLightingSample& NewLowQualitySample);

	/** Shrinks the octree and updates memory stats. */
	void FinalizeSamples();

	/** Invalidates anything produced by the last lighting build. */
	void InvalidateLightingCache();

	SIZE_T GetAllocatedBytes() const;

	bool IsInitialized() const
	{
		return bInitialized;
	}

	FBox& GetBounds()
	{
		return Bounds;
	}

	bool bInitialized;
	FBox Bounds;

	/** Octree containing lighting samples to be used with high quality lightmaps. */
	FLightVolumeOctree HighQualityLightmapOctree;

	/** Octree containing lighting samples to be used with low quality lightmaps. */
	FLightVolumeOctree LowQualityLightmapOctree;
};


class FExtendedPrecomputedLightVolume : FPrecomputedLightVolume
{
public:
	FTransform InverseLevelTransform;
	FExposedPrecomputedLightVolumeData* ExposedData;
	FLightVolumeOctree* ExposedOctreeForRendering;

public:
	FExtendedPrecomputedLightVolume();

	void InterpolateIncidentRadiancePoint(const FVector& Position, float& AccumulatedWeight, float& AccumulatedDirectionalLightShadowing, FSHVectorRGB3& AccumulatedIncidentRadiance, FVector& SkyBentNormal) const;
	void InterpolateIncidentRadianceBlock(const FBoxCenterAndExtent& BoundingBox, const FIntVector& QueryCellDimensions, const FIntVector& DestCellDimensions, const FIntVector& DestCellPosition, TArray<float>& AccumulatedWeights, TArray<FSHVectorRGB2>& AccumulatedIncidentRadiance) const;
	void DebugDrawSamples(class FPrimitiveDrawInterface* PDI, bool bDrawDirectionalShadowing) const;
	void ApplyLevelTransform(const FTransform& LevelTransform);
	void UpdateExposedData(FSceneInterface* Scene);
};
