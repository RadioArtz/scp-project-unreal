

#pragma once 

#include "CoreMinimal.h"

class FPrecomputedVolumetricLightmapData;

class SCPPU_API FVolumetricLightmapDataAccessHelper
{
	//// Functions ////
public:
	FVolumetricLightmapDataAccessHelper(FPrecomputedVolumetricLightmapData* InData);

	FVector GetAmbientVector(const FVector WorldLocation) const;
	FVector GetSkyBentNormal(const FVector WorldLocation) const;
	FSHVectorRGB3 GetLightColor(const FVector WorldLocation) const;
	float GetDirectionalLightShadowing(const FVector WorldLocation) const;

private:
	FVector GetBrickTextureCoordinate(const FVector WorldLocation) const;

	//// Properties ////
private:
	const FPrecomputedVolumetricLightmapData* Data;
};

