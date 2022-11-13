

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
		FString Formula = "(BaseValue + Sum of all Additions) * (1 + Sum of all Multiplications) * (1 - Sum of all Reductions)";
};

UCLASS(Config = Game, defaultconfig, meta = (DisplayName = "Attribute Float Modifier Settings"))
class SCPPU_API UAttributeFloatModifierSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly)
		TSoftObjectPtr<UDataTable> ModifierDataTable;
};