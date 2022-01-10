

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LayoutGenerator_Structs.h"
#include "LayoutGenerator_Main.h"
#include "LayoutGenerator_SpawnValidator.generated.h"

UCLASS(Blueprintable)
class SCPPU_API ULayoutGenerator_SpawnValidator : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
		bool IsValidSpawn();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		ALayoutGenerator_Main* LayoutGenerator;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FIntVector2D GridLocation;
};
