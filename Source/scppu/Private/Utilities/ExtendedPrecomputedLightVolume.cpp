


#include "Utilities/ExtendedPrecomputedLightVolume.h"
#include "PrecomputedLightVolume.h"

FExtendedPrecomputedLightVolume::FExtendedPrecomputedLightVolume()
{
	//FPrecomputedLightVolume::FPrecomputedLightVolume();
}

void FExtendedPrecomputedLightVolume::InterpolateIncidentRadiancePoint(const FVector& Position, float& AccumulatedWeight, float& AccumulatedDirectionalLightShadowing, FSHVectorRGB3& AccumulatedIncidentRadiance, FVector& SkyBentNormal) const
{
	// This could be called on a NULL volume for a newly created level. This is now checked at the call site, but this check provides extra safety
	checkf(this, TEXT("FPrecomputedLightVolume::InterpolateIncidentRadiancePoint() is called on a null volume. Fix the call site."));

	// Handle being called on a volume that hasn't been initialized yet, which can happen if lighting hasn't been built yet.
	if (ExposedData->bInitialized)
	{
		FVector WorldPosition = this->InverseLevelTransform.TransformPosition(Position); // relocate from world to volume space
		FBoxCenterAndExtent BoundingBox(WorldPosition, FVector::ZeroVector);
		// Iterate over the octree nodes containing the query point.
		ExposedOctreeForRendering->FindElementsWithBoundsTest(BoundingBox, [&AccumulatedIncidentRadiance, &SkyBentNormal, &AccumulatedDirectionalLightShadowing, &AccumulatedWeight, &WorldPosition](const FVolumeLightingSample& VolumeSample)
			{
				const float DistanceSquared = (VolumeSample.Position - WorldPosition).SizeSquared();
		const float RadiusSquared = FMath::Square(VolumeSample.Radius);

		if (DistanceSquared < RadiusSquared)
		{
			const float InvRadiusSquared = 1.0f / RadiusSquared;
			// Weight each sample with the fraction that Position is to the center of the sample, and inversely to the sample radius.
			// The weight goes to 0 when Position is on the bounding radius of the sample, so the interpolated result is continuous.
			// The sample's radius size is a factor so that smaller samples contribute more than larger, low detail ones.
			const float SampleWeight = (1.0f - DistanceSquared * InvRadiusSquared) * InvRadiusSquared;
			// Accumulate weighted results and the total weight for normalization later
			AccumulatedIncidentRadiance += VolumeSample.Lighting * SampleWeight;
			SkyBentNormal += VolumeSample.GetSkyBentNormalUnpacked() * SampleWeight;
			AccumulatedDirectionalLightShadowing += VolumeSample.DirectionalLightShadowing * SampleWeight;
			AccumulatedWeight += SampleWeight;
		}
			});
	}
}

void FExtendedPrecomputedLightVolume::InterpolateIncidentRadianceBlock(const FBoxCenterAndExtent& BoundingBox, const FIntVector& QueryCellDimensions, const FIntVector& DestCellDimensions, const FIntVector& DestCellPosition, TArray<float>& AccumulatedWeights, TArray<FSHVectorRGB2>& AccumulatedIncidentRadiance) const
{
	// This could be called on a NULL volume for a newly created level. This is now checked at the callsite, but this check provides extra safety
	checkf(this, TEXT("FPrecomputedLightVolume::InterpolateIncidentRadianceBlock() is called on a null volume. Fix the call site."));

	// Handle being called on a volume that hasn't been initialized yet, which can happen if lighting hasn't been built yet.
	if (ExposedData->bInitialized)
	{
		FBoxCenterAndExtent BoundingBox = BoundingBox.GetBox().TransformBy(this->InverseLevelTransform);
		BoundingBox.Center = BoundingBox.Center; // relocate from world to volume space

		static TArray<const FVolumeLightingSample*> PotentiallyIntersectingSamples;
		PotentiallyIntersectingSamples.Reset(100);

		// Iterate over the octree nodes containing the query point.
		ExposedOctreeForRendering->FindElementsWithBoundsTest(BoundingBox, [&](const FVolumeLightingSample& VolumeSample)
			{
				PotentiallyIntersectingSamples.Add(&VolumeSample);
			});

		const int32 LinearIndexBase = DestCellPosition.Z * DestCellDimensions.Y * DestCellDimensions.X
			+ DestCellPosition.Y * DestCellDimensions.X
			+ DestCellPosition.X;

		const int32 CenterIndex = LinearIndexBase + (QueryCellDimensions.Z / 2 * DestCellDimensions.Y + QueryCellDimensions.Y / 2) * DestCellDimensions.X + QueryCellDimensions.X / 2;

		for (int32 SampleIndex = 0; SampleIndex < PotentiallyIntersectingSamples.Num(); SampleIndex++)
		{
			const FVolumeLightingSample& VolumeSample = *PotentiallyIntersectingSamples[SampleIndex];

			const float RadiusSquared = FMath::Square(VolumeSample.Radius);
			const float WeightBase = 1.0f / RadiusSquared;
			const float WeightMultiplier = -1.0f / (RadiusSquared * RadiusSquared);

			const FVector BaseTranslationFromSample = BoundingBox.Center - BoundingBox.Extent - VolumeSample.Position;
			const FVector QuerySteps = BoundingBox.Extent / FVector(QueryCellDimensions) * 2;
			FVector TranslationFromSample = BaseTranslationFromSample;

			for (int32 Z = 0; Z < QueryCellDimensions.Z; Z++)
			{
				for (int32 Y = 0; Y < QueryCellDimensions.Y; Y++)
				{
					for (int32 X = 0; X < QueryCellDimensions.X; X++)
					{
						const float DistanceSquared = TranslationFromSample.SizeSquared();

						if (DistanceSquared < RadiusSquared)
						{
							const int32 LinearIndex = LinearIndexBase + (Z * DestCellDimensions.Y + Y) * DestCellDimensions.X + X;
							// Weight each sample with the fraction that Position is to the center of the sample, and inversely to the sample radius.
							// The weight goes to 0 when Position is on the bounding radius of the sample, so the interpolated result is continuous.
							// The sample's radius size is a factor so that smaller samples contribute more than larger, low detail ones.
							const float SampleWeight = DistanceSquared * WeightMultiplier + WeightBase;
							// Accumulate weighted results and the total weight for normalization later													
							AccumulatedIncidentRadiance[LinearIndex] += TSHVectorRGB<2>(VolumeSample.Lighting) * SampleWeight;
							AccumulatedWeights[LinearIndex] += SampleWeight;
						}

						TranslationFromSample.X += QuerySteps.X;
					}

					TranslationFromSample.X = BaseTranslationFromSample.X;
					TranslationFromSample.Y += QuerySteps.Y;
				}

				TranslationFromSample.Y = BaseTranslationFromSample.Y;
				TranslationFromSample.Z += QuerySteps.Z;
			}
		}
	}
}

void FExtendedPrecomputedLightVolume::DebugDrawSamples(FPrimitiveDrawInterface* PDI, bool bDrawDirectionalShadowing) const
{
	this->ExposedOctreeForRendering->FindAllElements([bDrawDirectionalShadowing, PDI, this](const FVolumeLightingSample& VolumeSample)
		{
			const FLinearColor AverageColor = bDrawDirectionalShadowing
			? FLinearColor(VolumeSample.DirectionalLightShadowing, VolumeSample.DirectionalLightShadowing, VolumeSample.DirectionalLightShadowing)
		: VolumeSample.Lighting.CalcIntegral() / (FSHVector2::ConstantBasisIntegral * PI);

	FVector SamplePosition = this->InverseLevelTransform.Inverse().TransformPosition(VolumeSample.Position); //relocate from volume to world space
	PDI->DrawPoint(SamplePosition, AverageColor, 10, SDPG_World);
		});
}

void FExtendedPrecomputedLightVolume::ApplyLevelTransform(const FTransform& LevelTransform)
{
	this->InverseLevelTransform = LevelTransform.Inverse();
}

void FExtendedPrecomputedLightVolume::UpdateExposedData(FSceneInterface* Scene)
{
	this->ExposedData = (FExposedPrecomputedLightVolumeData*)this->Data;
	this->ExposedOctreeForRendering = AllowHighQualityLightmaps(Scene->GetFeatureLevel()) ? &this->ExposedData->HighQualityLightmapOctree : &this->ExposedData->LowQualityLightmapOctree;
}

FExposedPrecomputedLightVolumeData::FExposedPrecomputedLightVolumeData() :
	bInitialized(false),
	HighQualityLightmapOctree(FVector::ZeroVector, HALF_WORLD_MAX),
	LowQualityLightmapOctree(FVector::ZeroVector, HALF_WORLD_MAX)
{}

FExposedPrecomputedLightVolumeData::~FExposedPrecomputedLightVolumeData()
{
	if (bInitialized)
	{
		const SIZE_T VolumeBytes = GetAllocatedBytes();
	}
}

void FExposedPrecomputedLightVolumeData::Initialize(const FBox& NewBounds)
{
	InvalidateLightingCache();
	bInitialized = true;
	Bounds = NewBounds;
	// Initialize the octree based on the passed in bounds
	HighQualityLightmapOctree = FLightVolumeOctree(NewBounds.GetCenter(), NewBounds.GetExtent().GetMax());
	LowQualityLightmapOctree = FLightVolumeOctree(NewBounds.GetCenter(), NewBounds.GetExtent().GetMax());
}

/*
void FExposedPrecomputedLightVolumeData::AddHighQualityLightingSample(const FVolumeLightingSample& NewHighQualitySample)
{
	check(bInitialized);
	HighQualityLightmapOctree.AddElement(NewHighQualitySample);
}

void FExposedPrecomputedLightVolumeData::AddLowQualityLightingSample(const FVolumeLightingSample& NewLowQualitySample)
{
	check(bInitialized);
	LowQualityLightmapOctree.AddElement(NewLowQualitySample);
}
*/
void FExposedPrecomputedLightVolumeData::FinalizeSamples()
{
	check(bInitialized);
	// No more samples will be added, shrink octree node element arrays
	HighQualityLightmapOctree.ShrinkElements();
	LowQualityLightmapOctree.ShrinkElements();
	const SIZE_T VolumeBytes = GetAllocatedBytes();
}

void FExposedPrecomputedLightVolumeData::InvalidateLightingCache()
{
	if (bInitialized)
	{
		// Release existing samples
		const SIZE_T VolumeBytes = GetAllocatedBytes();
		HighQualityLightmapOctree.Destroy();
		LowQualityLightmapOctree.Destroy();
		bInitialized = false;
	}
}

SIZE_T FExposedPrecomputedLightVolumeData::GetAllocatedBytes() const
{
	SIZE_T NodeBytes = 0;
	NodeBytes += HighQualityLightmapOctree.GetSizeBytes();
	NodeBytes += LowQualityLightmapOctree.GetSizeBytes();
	return NodeBytes;
}

FArchive& operator<<(FArchive& Ar, FExposedPrecomputedLightVolumeData& Volume)
{
	// TODO: hier return-Anweisung eingeben
	ensure(false);
	return Ar;
}

FArchive& operator<<(FArchive& Ar, FExposedPrecomputedLightVolumeData*& Volume)
{
	// TODO: hier return-Anweisung eingeben
	ensure(false);
	return Ar;
}
