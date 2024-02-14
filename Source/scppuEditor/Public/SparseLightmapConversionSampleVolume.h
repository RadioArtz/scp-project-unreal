

#pragma once 

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/Volume.h"
#include "ActorFactories/ActorFactoryBoxVolume.h" 
#include "SparseLightmapConversionSampleVolume.generated.h"

class AActor;
struct FAssetData;

UCLASS()
class SCPPUEDITOR_API UActorFactorySparseLightmapConversionSampleVolume : public UActorFactoryBoxVolume
{
	GENERATED_UCLASS_BODY()

private:
	virtual bool CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg) override;
	virtual void PostSpawnActor(UObject* Asset, AActor* NewActor) override;
};

UCLASS(hidecategories = (Collision, Brush, Attachment, Physics, Volume, Cooking, LOD, Replication))
class SCPPUEDITOR_API ASparseLightmapConversionSampleVolume : public AVolume
{
	GENERATED_UCLASS_BODY()

public:
	virtual bool IsLevelBoundsRelevant() const override;
};

