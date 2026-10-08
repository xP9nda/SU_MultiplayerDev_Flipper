// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Flipper : ModuleRules
{
	public Flipper(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] {
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"AIModule",
			"StateTreeModule",
			"GameplayStateTreeModule",
			"UMG",
			"Slate"
		});

		PrivateDependencyModuleNames.AddRange(new string[] { "GoogleTest" });

		PublicIncludePaths.AddRange(new string[] {
			"Flipper",
			"Flipper/Variant_Horror",
			"Flipper/Variant_Horror/UI",
			"Flipper/Variant_Shooter",
			"Flipper/Variant_Shooter/AI",
			"Flipper/Variant_Shooter/UI",
			"Flipper/Variant_Shooter/Weapons"
		});

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
