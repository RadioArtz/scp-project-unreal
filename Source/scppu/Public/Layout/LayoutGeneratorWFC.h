

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
			this->Rotation = InRotation % 4;
		}

		friend bool operator== (const FCellPossibility& Self, const FCellPossibility& Other)
		{
			return (Self.RowName == Other.RowName) && (Self.Rotation == Other.Rotation);
		}

		FString ToString()
		{
			return FString::Printf(TEXT("[Row: %s, Rotation: %i]"), *this->RowName.ToString(), this->Rotation);
		}
	};


	//// Functions ////
public:
	UFUNCTION(BlueprintCallable)
		bool Generate(ALayout* Layout, int32 NewSeed);

	UFUNCTION(BlueprintCallable, meta = (AutoCreateRefTerm = "OnDone"))
		void AsyncGenerate(ALayout* Layout, int32 NewSeed, FLayoutGeneratorWFCDone OnDone);

protected:
	UFUNCTION()
		bool GenerateInternal(ALayout* Layout, int32 Seed);
};
