

#pragma once

#include "LayoutStructs.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LayoutSpawnValidator.generated.h"

class ALayout;
class ULayoutCell;

UCLASS(Blueprintable, Abstract)
class SCPPU_API ULayoutSpawnValidator : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
		bool IsValidSpawn(ALayout* Layout, ULayoutCell* Cell, FRandomStream RandStream);
};
