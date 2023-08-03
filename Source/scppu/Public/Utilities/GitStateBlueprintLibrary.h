

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GitStateBlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SCPPU_API UGitStateBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	//// Functions ////	
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		static FString GetActiveGitCommitHash();

	UFUNCTION(BlueprintCallable, BlueprintPure)
		static bool HasActiveGitCommitLocalChanges();
};
