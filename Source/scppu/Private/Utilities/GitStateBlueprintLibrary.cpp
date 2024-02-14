


#include "Utilities/GitStateBlueprintLibrary.h"

#ifndef PROJECT_GIT_COMMIT_HASH
    #define PROJECT_GIT_COMMIT_HASH "INVALID"
#endif

#ifndef PROJECT_GIT_HAS_COMMIT_LOCAL_CHANGES
    #define PROJECT_GIT_HAS_COMMIT_LOCAL_CHANGES 0
#endif


FString UGitStateBlueprintLibrary::GetActiveGitCommitHash()
{
    return FString(PROJECT_GIT_COMMIT_HASH);
}

bool UGitStateBlueprintLibrary::HasActiveGitCommitLocalChanges()
{
#if PROJECT_GIT_HAS_COMMIT_LOCAL_CHANGES
    return true;
#else
    return false;
#endif
}
