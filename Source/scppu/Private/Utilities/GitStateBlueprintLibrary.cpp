


#include "Utilities/GitStateBlueprintLibrary.h"

FString UGitStateBlueprintLibrary::GetActiveGitCommitHash()
{
#if defined PROJECT_GIT_COMMIT_HASH
    return FString(PROJECT_GIT_COMMIT_HASH);
#else
    return FString("UNKNOWN");
#endif
}

bool UGitStateBlueprintLibrary::HasActiveGitCommitLocalChanges()
{
#if PROJECT_GIT_HAS_COMMIT_LOCAL_CHANGES
    return true;
#else
    return false;
#endif
}
