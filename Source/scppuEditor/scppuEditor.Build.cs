

using UnrealBuildTool;
using System;
using System.Diagnostics;

public class scppuEditor : ModuleRules
{
	public scppuEditor(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[] { "scppu", "Core", "CoreUObject", "Engine", "InputCore", "UMG", "DeveloperSettings", "RenderCore", "Renderer", "RHI", "Http",  "Slate", "SlateCore", "EditorSubsystem", "UnrealEd", "GpuLightmass", "PropertyEditor", "EditorStyle"});
		PrivateDependencyModuleNames.AddRange(new string[] { });
	}
}
