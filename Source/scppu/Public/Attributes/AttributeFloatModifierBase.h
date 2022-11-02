

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AttributeFloatModifierBase.generated.h"

UCLASS(Blueprintable, Abstract)
class SCPPU_API UAttributeFloatModifierBase : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
		float GetModifiedValue(float BaseValue);
};
