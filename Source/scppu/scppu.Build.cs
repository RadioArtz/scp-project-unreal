

using UnrealBuildTool;
using System;
using System.Diagnostics;

public class scppu : ModuleRules
{
	public scppu(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "UMG", "DeveloperSettings", "RenderCore", "Renderer", "RHI", "Http",  "Slate", "SlateCore"});
		PrivateDependencyModuleNames.AddRange(new string[] {  });
		bEnableUndefinedIdentifierWarnings = false;
		bool bShouldDeterminGitStatus = true;

		// Determin git active commit hash and state to be used in engine (see UGitStateBlueprintLibrary for more information)
		if (bShouldDeterminGitStatus)
		{
			try
			{
				ProcessStartInfo StartInfo = new ProcessStartInfo();
				StartInfo.FileName = "git";
				StartInfo.UseShellExecute = false;
				StartInfo.WorkingDirectory = ModuleDirectory;
				StartInfo.RedirectStandardOutput = true;
				StartInfo.RedirectStandardError = true;
				Process Proc;

				// Determin commit hash
				Console.WriteLine("Using 'git show' to determine commit hash to be used in engine");
				StartInfo.Arguments = "show --quiet --format=%H";
				Proc = Process.Start(StartInfo);
				Proc.WaitForExit();
				string GitCommitHash = Proc.StandardOutput.ReadToEnd().Trim();
				Console.WriteLine("Detected git commit hash: " + GitCommitHash);

				// Determin if local changes exist
				Console.WriteLine("Using 'git status' to determine commit hash to be used in engine");
				StartInfo.Arguments = "status --porcelain";
				Proc = Process.Start(StartInfo);
				Proc.WaitForExit();
				bool bHasLocalChanges = Proc.StandardOutput.ReadToEnd().Length > 0;
				Console.WriteLine("Detected git has local changes: " + bHasLocalChanges);

				// Add definitions to be used in cpp code
				PublicDefinitions.Add("PROJECT_GIT_COMMIT_HASH=\"" + GitCommitHash + "\"");
				PublicDefinitions.Add("PROJECT_GIT_HAS_COMMIT_LOCAL_CHANGES=" + (bHasLocalChanges ? "1" : "0"));
			}
			catch (Exception Exp)
			{
				Console.WriteLine("Something went wrong determining git status: " + Exp.Message);
			}
		}
		else
		{
			Console.WriteLine("Not determining git status to be used in engine");
		}
	}
}
