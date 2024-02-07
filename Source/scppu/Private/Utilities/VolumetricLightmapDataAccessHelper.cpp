#include "Utilities/VolumetricLightmapDataAccessHelper.h"
#include "PrecomputedVolumetricLightmap.h"

FVolumetricLightmapDataAccessHelper::FVolumetricLightmapDataAccessHelper(FPrecomputedVolumetricLightmapData* InData)
{
	check(InData);
	InData->BrickData.SetNeedsCPUAccess(true);
	this->Data = InData;
}

const FVector FVolumetricLightmapDataAccessHelper::GetAmbientVector(const FVector WorldLocation)
{
    const FVector BrickTextureCoordinate = this->GetBrickTextureCoordinate(WorldLocation);
    const FVector AmbientVector = (FVector)FilteredVolumeLookup<FFloat3Packed>(BrickTextureCoordinate, this->Data->BrickDataDimensions, (const FFloat3Packed*)this->Data->BrickData.AmbientVector.Data.GetData());
    return AmbientVector;
}

const FVector FVolumetricLightmapDataAccessHelper::GetSkyBentNormal(const FVector WorldLocation)
{
    if (this->Data->BrickData.SkyBentNormal.Data.Num() <= 0)
    {
        return FLinearColor::Black;
    }

    const FVector BrickTextureCoordinate = this->GetBrickTextureCoordinate(WorldLocation);
    const FLinearColor SkyBentNormal = FilteredVolumeLookup<FColor>(BrickTextureCoordinate, this->Data->BrickDataDimensions, (const FColor*)this->Data->BrickData.SkyBentNormal.Data.GetData());
    return FVector(SkyBentNormal.R, SkyBentNormal.G, SkyBentNormal.B);
}

const FSHVectorRGB3 FVolumetricLightmapDataAccessHelper::GetLightColor(const FVector WorldLocation)
{
	const FVector BrickTextureCoordinate = this->GetBrickTextureCoordinate(WorldLocation);
	const FVector AmbientVector = this->GetAmbientVector(WorldLocation);
	const FPrecomputedVolumetricLightmapData* DataLambda = this->Data;

	auto ReadSHCoefficient = [&BrickTextureCoordinate, &DataLambda, &AmbientVector](uint32 CoefficientIndex)
	{
		check(CoefficientIndex < UE_ARRAY_COUNT(DataLambda->BrickData.SHCoefficients));

		// Undo normalization done in FIrradianceBrickData::SetFromVolumeLightingSample
		const FLinearColor SHDenormalizationScales0(
			0.488603f / 0.282095f,
			0.488603f / 0.282095f,
			0.488603f / 0.282095f,
			1.092548f / 0.282095f);

		const FLinearColor SHDenormalizationScales1(
			1.092548f / 0.282095f,
			4.0f * 0.315392f / 0.282095f,
			1.092548f / 0.282095f,
			2.0f * 0.546274f / 0.282095f);

		FLinearColor SHCoefficientEncoded = FilteredVolumeLookup<FColor>(BrickTextureCoordinate, DataLambda->BrickDataDimensions, (const FColor*)DataLambda->BrickData.SHCoefficients[CoefficientIndex].Data.GetData());
		
		//Swap R and B channel because it was swapped at ImportVolumetricLightmap for changing format from BGRA to RGBA
		Swap(SHCoefficientEncoded.R, SHCoefficientEncoded.B);

		const FLinearColor& DenormalizationScales = ((CoefficientIndex & 1) == 0) ? SHDenormalizationScales0 : SHDenormalizationScales1;
		return FVector4((SHCoefficientEncoded * 2.0f - FLinearColor(1.0f, 1.0f, 1.0f, 1.0f)) * AmbientVector[CoefficientIndex / 2] * DenormalizationScales);
	};

	auto GetSHVector3 = [](float Ambient, const FVector4& Coeffs0, const FVector4& Coeffs1)
	{
		FSHVector3 Result;
		Result.V[0] = Ambient;
		FMemory::Memcpy(&Result.V[1], &Coeffs0, sizeof(Coeffs0));
		FMemory::Memcpy(&Result.V[5], &Coeffs1, sizeof(Coeffs1));
		return Result;
	};

	FSHVectorRGB3 LQSH;
	LQSH.R = GetSHVector3(AmbientVector.X, ReadSHCoefficient(0), ReadSHCoefficient(1));
	LQSH.G = GetSHVector3(AmbientVector.Y, ReadSHCoefficient(2), ReadSHCoefficient(3));
	LQSH.B = GetSHVector3(AmbientVector.Z, ReadSHCoefficient(4), ReadSHCoefficient(5));

	if (this->Data->BrickData.LQLightColor.Data.Num() > 0)
	{
		// Add stationary direct lighting to match ILC's LQ behavior.
		FLinearColor LQLightColor = FilteredVolumeLookup<FFloat3Packed>(BrickTextureCoordinate, this->Data->BrickDataDimensions, (const FFloat3Packed*)this->Data->BrickData.LQLightColor.Data.GetData());
		FVector LQLightDirection = (FVector)FilteredVolumeLookup<FColor>(BrickTextureCoordinate, this->Data->BrickDataDimensions, (const FColor*)this->Data->BrickData.LQLightDirection.Data.GetData());

		//Swap X and Z channel because it was swapped at ImportVolumetricLightmap for changing format from BGRA to RGBA
		Swap(LQLightDirection.X, LQLightDirection.Z);
		LQLightDirection = LQLightDirection * 2.0f - FVector(1.0f, 1.0f, 1.0f);
		LQLightDirection.Normalize();

		LQSH.AddIncomingRadiance(LQLightColor, 1.0f, LQLightDirection);
	}

	return LQSH;
}

const float FVolumetricLightmapDataAccessHelper::GetDirectionalLightShadowing(const FVector WorldLocation)
{
	const FVector BrickTextureCoordinate = this->GetBrickTextureCoordinate(WorldLocation);
	const FLinearColor DirectionalLightShadowing = FilteredVolumeLookup<uint8>(BrickTextureCoordinate, this->Data->BrickDataDimensions, (const uint8*)this->Data->BrickData.DirectionalLightShadowing.Data.GetData());
	return DirectionalLightShadowing.R;
}

const FVector FVolumetricLightmapDataAccessHelper::GetBrickTextureCoordinate(const FVector WorldLocation)
{
    const FVector IndirectionDataSourceCoordinate = ComputeIndirectionCoordinate(WorldLocation, this->Data->GetBounds(), this->Data->IndirectionTextureDimensions);
    
    FIntVector IndirectionBrickOffset;
    int32 IndirectionBrickSize;
    SampleIndirectionTexture(IndirectionDataSourceCoordinate, this->Data->IndirectionTextureDimensions, this->Data->IndirectionTexture.Data.GetData(), IndirectionBrickOffset, IndirectionBrickSize);

    const FVector BrickTextureCoordinate = ComputeBrickTextureCoordinate(IndirectionDataSourceCoordinate, IndirectionBrickOffset, IndirectionBrickSize, this->Data->BrickSize);
    return BrickTextureCoordinate;
}
