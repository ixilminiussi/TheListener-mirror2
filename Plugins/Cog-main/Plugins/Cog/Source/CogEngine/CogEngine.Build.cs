using System;
using UnrealBuildTool;

public class CogEngine : ModuleRules
{
	public CogEngine(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(
			new[]
			{
				"CogDebug"
			});

		PrivateDependencyModuleNames.AddRange(
			new[]
			{
				"ApplicationCore",
				"CogCommon",
				"CogImgui",
				"Cog",
				"Core",
				"CoreUObject",
				"Engine",
				"InputCore",
				"NetCore",
				"Slate",
				"SlateCore",
				"BuildSettings"
			});

		if (Target.bBuildEditor)
			PrivateDependencyModuleNames.AddRange(
				new[]
				{
					"AssetTools"
				});

		PublicDefinitions.Add("BUILD_TIMESTAMP=\"" + DateTime.UtcNow.ToString("dd MMM yyyy") + "\"");
	}
}