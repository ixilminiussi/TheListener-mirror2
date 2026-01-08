// Copyright (c) 2025 Rafael Valoto/Publisher. All rights reserved.
// Created for: WindowsDualsense_ds5w - Plugin to support DualSense controller on Windows.
// Planned Release Year: 2025


using System.IO;
using UnrealBuildTool;

public class WindowsDualsense_ds5w : ModuleRules
{
	public WindowsDualsense_ds5w(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
 		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "ApplicationCore", "InputCore", "InputDevice",  "AudioMixer" });
	    PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
	    bEnableExceptions = true;
	    
	    if (Target.Platform == UnrealTargetPlatform.Win64)
	    {
		    string hidPath = @"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.26100.0\um\x64\hid.lib";
		    
		    if (System.IO.File.Exists(hidPath))
		    {
			    PublicAdditionalLibraries.Add(hidPath);
		    }
		    else
		    {
				PublicAdditionalLibraries.Add("hid.lib");
		    }
	    }
	    
	    if (Target.Platform == UnrealTargetPlatform.Linux || Target.Platform == UnrealTargetPlatform.Mac)
	    {
		    PrivateDependencyModuleNames.AddRange(new string[] { "SDL2" });
	    }
	}
}
