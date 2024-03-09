

#pragma once

#include "CoreMinimal.h"

#ifndef GIT_COMPILED_COMMIT_HASH
	#define GIT_COMPILED_COMMIT_HASH "NO COMMIT HASH"
#endif

#ifndef GIT_COMPILED_COMMIT_HAS_LOCAL_CHANGES
	#define GIT_COMPILED_COMMIT_HAS_LOCAL_CHANGES 0
#endif

class SCPPUGIT_API FScppuGitModule : public IModuleInterface
{

public:
	const FString CompiledGitCommitHash = GIT_COMPILED_COMMIT_HASH;
	const bool bCompiledGitCommitHasLocalChanges = GIT_COMPILED_COMMIT_HAS_LOCAL_CHANGES;

public:
	void StartupModule() override;
	void ShutdownModule() override;
};
