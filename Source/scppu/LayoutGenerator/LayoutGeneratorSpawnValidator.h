

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LayoutGeneratorStructs.h"
#include "LayoutGeneratorCell.h"
#include "LayoutGeneratorSpawnValidator.generated.h"

UCLASS(Blueprintable, Abstract)
class SCPPU_API ULayoutGeneratorSpawnValidator : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
		bool IsSpawnValid(ALayoutGeneratorMain* LayoutGenerator, FIntVector2D Location);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FRandomStream RandomStream;

};
