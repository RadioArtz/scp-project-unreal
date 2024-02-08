

#pragma once 

#include "CoreMinimal.h"

class FPrecomputedVolumetricLightmapData;

class SCPPU_API FVolumetricLightmapDataAccessHelper
{
	//// Functions ////
public:
	FVolumetricLightmapDataAccessHelper(FPrecomputedVolumetricLightmapData* InData);

	const FVector GetAmbientVector(const FVector WorldLocation);
	const FVector GetSkyBentNormal(const FVector WorldLocation);
	const FSHVectorRGB3 GetLightColor(const FVector WorldLocation);
	const float GetDirectionalLightShadowing(const FVector WorldLocation);

private:
	const FVector GetBrickTextureCoordinate(const FVector WorldLocation);

	//// Properties ////
private:
	const FPrecomputedVolumetricLightmapData* Data;
};

