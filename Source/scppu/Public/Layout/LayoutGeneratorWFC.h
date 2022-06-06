

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LayoutGeneratorWFC.generated.h"

class ALayout;

UCLASS()
class SCPPU_API ULayoutGeneratorWFC : public UObject
{
	GENERATED_BODY()

public:
	//// Properties ////
	
public:
	//// Functions ////
	UFUNCTION(BlueprintCallable)
		bool GenerateLayout(ALayout* Layout, int32 NewSeed);

	UFUNCTION(BlueprintCallable)
		void GenerateLayoutAsync(ALayout* Layout, int32 NewSeed);
};
