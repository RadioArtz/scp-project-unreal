
#pragma once

#include "CoreMinimal.h"

#include "Subsystems/WorldSubsystem.h"
#include "Engine/MapBuildDataRegistry.h" 
#include "SparseLightmapConversionSubsystem.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSparseLightmapConversionSubsystem, Log, All);

class AExtendedWorldSettings;
class FPrecomputedLightVolumeData;
class FPrecomputedVolumetricLightmapData;

UCLASS(Transient)
class SCPPUEDITOR_API USparseLightmapConversionSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

	//// Functions ////
public:
	virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual TStatId GetStatId() const override { return this->GetStatID(); }
	virtual bool IsTickableInEditor() const override { return true; }
	virtual void Tick(float DeltaTime) override;
	virtual void Deinitialize() override;

	void OnLightBuildStarted();
	void OnLightBuildCompleted();

private:
	bool IsAnyLightmassBuildActive();
	ULevel* GetActivePersistentLevel();
	AExtendedWorldSettings* GetActiveWorldSettingsChecked();
	void ClearMapRegistryPrecomputedLightmapEntries(UMapBuildDataRegistry* MapRegistry, const FGuid LevelBuildDataId);
	bool IsPointIntersectingStaticGeometry(FVector Point);
	bool IsPointAboveFloor(FVector Point);
	FPrecomputedLightVolumeData* CreateLightVolumeDataFromVolumetricLightmapData(FPrecomputedVolumetricLightmapData* VolumetricLightmapData);
	void DeleteLightVolumeData(FPrecomputedLightVolumeData* Data);

	//// Properties ////
private:
	bool bConversionScheduled = false;

	//// Types ////
private:

	// Type punned version of UMapBuildDataRegistry.
	class UPunnedMapBuildDataRegistry : UObject
	{
	public:
		TEnumAsByte<enum ELightingBuildQuality> LevelLightingQuality;

		TMap<FGuid, FMeshMapBuildData> MeshBuildData;
		TMap<FGuid, FPrecomputedLightVolumeData*> LevelPrecomputedLightVolumeBuildData;
		TMap<FGuid, FPrecomputedVolumetricLightmapData*> LevelPrecomputedVolumetricLightmapBuildData;
		TMap<FGuid, FLightComponentMapBuildData> LightBuildData;
		TMap<FGuid, FReflectionCaptureMapBuildData> ReflectionCaptureBuildData;
		TMap<FGuid, FSkyAtmosphereMapBuildData> SkyAtmosphereBuildData;

		bool bSetupResourceClusters;
		TArray<FLightmapResourceCluster> LightmapResourceClusters;

		FRenderCommandFence DestroyFence;
	};

	// Used as a failsafe to catch bad type puns. 
	static_assert(sizeof(UPunnedMapBuildDataRegistry) == sizeof(UMapBuildDataRegistry), "Memory layout of UPunnedMapBuildDataRegistry must exactly match UMapBuildDataRegistry");
};

