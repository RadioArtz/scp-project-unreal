

using UnrealBuildTool;
using System;
using System.Diagnostics;

public class scppu : ModuleRules
{
	public scppu(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "DeveloperSettings", "RenderCore", "Renderer", "RHI", "Http",  "Slate", "SlateCore"});
		PrivateDependencyModuleNames.AddRange(new string[] { });
		CppStandard = CppStandardVersion.Cpp17;
	}
}
