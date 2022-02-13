

using UnrealBuildTool;
using System.Collections.Generic;

public class scppuEditorTarget : TargetRules
{
	public scppuEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V2;

		ExtraModuleNames.AddRange( new string[] { "scppu" } );

		bUseAdaptiveUnityBuild = true;
	}
}
