

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BaseLayoutSpawnValidator.generated.h"

class ALayout;
class ULayoutCell;

UCLASS(Blueprintable, Abstract)
class SCPPU_API UBaseLayoutSpawnValidator : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
		bool IsValidSpawn(ALayout* Layout, ULayoutCell* Cell, FRandomStream RandStream);
};
