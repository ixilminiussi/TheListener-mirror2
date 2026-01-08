using UnrealBuildTool;

public class TheListener : ModuleRules
{
	public TheListener(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[]
		{
			"Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "UMG", "CommonUI", "CommonInput", "AkAudio",
			"WwiseSoundEngine", "Inkpot", "InkPlusPlus", "Slate", "SlateCore", "CogCommon",
			"EngineCameras", "WindowsDualsense_ds5w",
			"EventGraphRuntime", "AudioSynesthesia", "Niagara","DialogueGraphRuntime"
		});
		
		// Add COM libs here
		PublicSystemLibraries.AddRange(new string[] { "Ole32.lib", "OleAut32.lib", "Uuid.lib" });

		if (Target.Configuration != UnrealTargetConfiguration.Shipping)
			PublicDependencyModuleNames.AddRange(new[]
			{
				"Cog",
				"CogAbility",
				"CogAI",
				"CogAll",
				"CogDebug",
				"CogEngine",
				"CogImgui",
				"CogInput"
			});
		if (Target.bBuildEditor)
		{
			PublicDependencyModuleNames.AddRange(new string[] {"EventGraph" });
			
		}
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}