

using UnrealBuildTool;
using System.Collections.Generic;

public class scppuTarget : TargetRules
{
	public scppuTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Game;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "scppu" } );

		bUseAdaptiveUnityBuild = true;
		WindowsPlatform.PCHMemoryAllocationFactor = 2000; // Heap memory allocation. 2000x => 1500 MB
	}
}
