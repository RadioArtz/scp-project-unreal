


#include "GitStateBlueprintLibrary.h"
#include "../scppuGit.h"

FString UGitStateBlueprintLibrary::GetCompiledGitCommitHash()
{
    const FScppuGitModule& GitModule = FModuleManager::Get().GetModuleChecked<FScppuGitModule>("scppuGit");
    return GitModule.CompiledGitCommitHash;
}

bool UGitStateBlueprintLibrary::CompiledGitCommitHasLocalChanges()
{
    const FScppuGitModule& GitModule = FModuleManager::Get().GetModuleChecked<FScppuGitModule>("scppuGit");
    return GitModule.bCompiledGitCommitHasLocalChanges;
}
