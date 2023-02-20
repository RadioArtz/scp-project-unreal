

#pragma once

#include "CoreMinimal.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "ExtendedPhysicalMaterial.generated.h"

UCLASS(BlueprintType, Blueprintable, CollapseCategories, HideCategories = Object)
class SCPPU_API UExtendedPhysicalMaterial : public UPhysicalMaterial
{
	GENERATED_BODY()
	
		//// Properties ////
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DynamicReverb, meta = (ClampMin = 0, UIMax = 1.f, DisplayAfter = SurfaceType))
		float HighFrequencyAbsorptionCoefficient = .08f;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = DynamicReverb, meta = (ClampMin = 0, UIMax = 1.f, DisplayAfter = SurfaceType))
		float LowFrequencyAbsorptionCoefficient = .08f;


	//// Functions ////	
public:
	// Tries to cast, if it fails will create a new extended physical material and copy the values from the in phsyical material over.
	// Returns always a valid reference expect when in phsyical material is nullptr or garbage collected.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static UExtendedPhysicalMaterial* ToExtendedPhysicalMaterial(UPhysicalMaterial* PhysicalMaterial);
};
