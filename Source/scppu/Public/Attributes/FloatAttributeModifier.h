

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "FloatAttributeModifier.generated.h"

UENUM()
enum EFloatAttributeModifierMode
{
	Addition = 0,
	Multiplication = 1,
	Reduction = 2
};

USTRUCT(BlueprintType)
struct FFloatAttributeModifier : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TEnumAsByte<EFloatAttributeModifierMode> Mode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Value;

private:
	UPROPERTY(VisibleAnywhere)
		FString Formula = "(Base value + Sum of all additon values) * (1 + Sum of all multiplications values) * (1 - Sum of all reduction values)";
};

UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Float Attribute Modifier"))
class SCPPU_API UFloatAttributeModifierSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
		TSoftObjectPtr<UDataTable> AttributeModifierDataTable;
};