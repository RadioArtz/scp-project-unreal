

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Layout/LayoutStructs.h"
#include "LayoutGeneratorWFC.generated.h"

class ALayout;

UDELEGATE(BlueprintCallable)
DECLARE_DYNAMIC_DELEGATE_OneParam(FLayoutGeneratorWFCDone, bool, bSuccess);

UCLASS(Blueprintable)
class SCPPU_API ULayoutGeneratorWFC : public UObject
{
	GENERATED_BODY()

protected:
	struct FCellPossibility
	{
	public:
		FName RowName;
		int32 Rotation;

		FCellPossibility(FName InRowName, int32 InRotation)
		{
			this->RowName = InRowName;
			this->Rotation = InRotation;
		}
	};

public:
	//// Properties ////
	
public:
	//// Functions ////
	UFUNCTION(BlueprintCallable)
		bool Generate(ALayout* Layout, int32 NewSeed);

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "OnDone"))
		void AsyncGenerate(ALayout* Layout, int32 NewSeed, FLayoutGeneratorWFCDone OnDone);

protected:
	UFUNCTION(BlueprintCallable)
		bool GenerateInternal(ALayout* Layout, int32 Seed);
};
