// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class MagicBattle : ModuleRules
{
	public MagicBattle(ReadOnlyTargetRules Target, bool usingSlateUI = false) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore" });

		PrivateDependencyModuleNames.AddRange(new string[] {  });
		// RLY? Make a flag for those who are using Slate UI
		if (usingSlateUI)
		{
			PrivateDependencyModuleNames.AddRange(new string[] {"Slate", "SlateCore"});
			PrivateDependencyModuleNames.Add("OnlineSubsystem");
		}
		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file.
		// With the Enabled attribute set to true
	}
}
