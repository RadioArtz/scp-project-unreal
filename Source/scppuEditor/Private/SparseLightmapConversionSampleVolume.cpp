#include "SparseLightmapConversionSampleVolume.h"
#include "Engine/CollisionProfile.h"
#include "Components/BrushComponent.h"

UActorFactorySparseLightmapConversionSampleVolume::UActorFactorySparseLightmapConversionSampleVolume(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	DisplayName = FText::FromString("Sparse Lightmap Conversion Sample Volume");
	NewActorClass = ASparseLightmapConversionSampleVolume::StaticClass();
}

bool UActorFactorySparseLightmapConversionSampleVolume::CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg)
{
	const FString PathName = ASparseLightmapConversionSampleVolume::StaticClass()->GetPathName();
	if (AssetData.IsValid() && AssetData.ObjectPath != FName(PathName))
	{
		return false;
	}

	return true;
}

void UActorFactorySparseLightmapConversionSampleVolume::PostSpawnActor(UObject* Asset, AActor* NewActor)
{
	Super::PostSpawnActor(Asset, NewActor);
}

ASparseLightmapConversionSampleVolume::ASparseLightmapConversionSampleVolume(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	this->GetBrushComponent()->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	this->bColored = true;
	this->BrushColor.R = 185;
	this->BrushColor.G = 25;
	this->BrushColor.B = 155;
	this->BrushColor.A = 255;
	this->bIsEditorOnlyActor = true;
}

bool ASparseLightmapConversionSampleVolume::IsLevelBoundsRelevant() const
{
	return true;
}

