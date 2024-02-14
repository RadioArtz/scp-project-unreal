

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "ExtendedWorldSettings.generated.h"

USTRUCT()
struct SCPPU_API FSparseLightmapConversionSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = SamplePlacement, meta = (ClampMin = 100.f, ClampMax = 10000.f))
		float SampleInterval = 200.f;

	UPROPERTY(EditAnywhere, Category = SamplePlacement)
		FVector SampleOffset;

	UPROPERTY(EditAnywhere, Category = SampleFiltering, AdvancedDisplay)
		bool bOnlySamplePointsOutsideGeometry = true;

	UPROPERTY(EditAnywhere, Category = SampleFiltering, AdvancedDisplay)
		bool bOnlySamplePointsAboveFloor = true;

	UPROPERTY(EditAnywhere, Category = SampleFiltering, AdvancedDisplay, meta = (ClampMin = 100.f, EditCondition = bOnlySamplePointsAboveFloor))
		float MaxHeightAboveFloor = 500.f;

	UPROPERTY(EditAnywhere, Category = LightmapBounds, AdvancedDisplay)
		bool bOverrideLightmapBounds = false;

	UPROPERTY(EditAnywhere, Category = LightmapBounds, AdvancedDisplay, meta = (EditCondition = bOverrideLightmapBounds))
		FBox LightmapBounds;
};

UCLASS()
class SCPPU_API AExtendedWorldSettings : public AWorldSettings
{
	GENERATED_BODY()

	//// Properties ////
public:
	UPROPERTY(EditAnywhere, Category = SparseLightmapConversion)
		bool bUseVolumetricLightmapAlgorithmForSparseLightmap = false;

	UPROPERTY(EditAnywhere, Category = SparseLightmapConversion)
		FSparseLightmapConversionSettings SparseLightmapConversionSettings;

	bool bUILockLightmassSettings = false;

	//// Functions ////
public:
#if WITH_EDITOR
		virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif
	
};