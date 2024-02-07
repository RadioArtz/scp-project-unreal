


#include "Utilities/ExtendedWorldSettings.h"

#if WITH_EDITOR
bool AExtendedWorldSettings::CanEditChange(const FProperty* InProperty) const
{
	if (!Super::CanEditChange(InProperty))
	{
		return false;
	}

	const bool bHasSparseLightingMethod = this->LightmassSettings.VolumeLightingMethod == VLM_SparseVolumeLightingSamples;

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(AExtendedWorldSettings, bUseVolumetricLightmapAlgorithmForSparseLightmap))
	{
		return bHasSparseLightingMethod;
	}

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(AExtendedWorldSettings, SparseLightmapConversionSettings))
	{
		return bHasSparseLightingMethod && this->bUseVolumetricLightmapAlgorithmForSparseLightmap;
	}

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(AExtendedWorldSettings, LightmassSettings))
	{
		return !this->bUILockLightmassSettings;
	}

	return true;
}
#endif
