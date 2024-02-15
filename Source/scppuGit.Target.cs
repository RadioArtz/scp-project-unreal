

using UnrealBuildTool;
using System.Collections.Generic;

public class scppuGitTarget : TargetRules
{
	public scppuGitTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Client;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "scppuGit" } );

		bUseAdaptiveUnityBuild = true;
		WindowsPlatform.PCHMemoryAllocationFactor = 2000; // Heap memory allocation. 2000x => 1500 MB
	}
}
