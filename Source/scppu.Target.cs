

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
	}
}
