

using UnrealBuildTool;

public class scppu : ModuleRules
{
	public scppu(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "DeveloperSettings", "RenderCore", "Renderer", "RHI", "Http",  "Slate", "SlateCore"});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
