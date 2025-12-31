#pragma once

#include "Inventory/BaseItem.h"
#include "SCPs/914/SCP914Roughness.h"
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SCP914Recipe.generated.h"

UCLASS(Blueprintable, Abstract)
class SCPPU_API USCP914Recipe : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void Evaluate914ItemRecipe(ABaseItem* InputItem, E914Roughness RoughnessLevel, bool& bIsValidRecipeForInput, TSubclassOf<ABaseItem>& OutputItem);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	bool Evaluate914CharacterRecipe(APawn* InputCharacter, E914Roughness RoughnessLevel);
};
