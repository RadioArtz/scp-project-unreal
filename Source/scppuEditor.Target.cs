

using UnrealBuildTool;
using System.Collections.Generic;

public class scppuEditorTarget : TargetRules
{
	public scppuEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;
		ExtraModuleNames.AddRange( new string[] { "scppu", "scppuEditor" } );

		bUseAdaptiveUnityBuild = true;
		WindowsPlatform.PCHMemoryAllocationFactor = 2000; // Heap memory allocation. 2000x => 1500 MB
	}
}
