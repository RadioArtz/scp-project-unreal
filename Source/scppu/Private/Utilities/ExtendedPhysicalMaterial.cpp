


#include "Utilities/ExtendedPhysicalMaterial.h"

UExtendedPhysicalMaterial* UExtendedPhysicalMaterial::ToExtendedPhysicalMaterial(UPhysicalMaterial* PhysicalMaterial)
{
	if (!IsValid(PhysicalMaterial))
	{
		return nullptr;
	}

	if (PhysicalMaterial->IsA(UExtendedPhysicalMaterial::StaticClass()))
	{
		return (UExtendedPhysicalMaterial*)PhysicalMaterial;
	}

	auto DuplicationParamters = FObjectDuplicationParameters(PhysicalMaterial, PhysicalMaterial->GetOuter());
	DuplicationParamters.DestClass = UExtendedPhysicalMaterial::StaticClass();
	return (UExtendedPhysicalMaterial*)StaticDuplicateObjectEx(DuplicationParamters);
}
