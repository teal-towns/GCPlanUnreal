// Fill out your copyright notice in the Description page of Project Settings.

using System;
using System.IO;

using UnrealBuildTool;

public class GCPlan : ModuleRules
{
	public GCPlan(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "..", "ThirdParty", "cppdelaunay", "libcppdelaunay.a"));
		PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "..", "ThirdParty", "cppdelaunay"));
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "WebSockets", "Json", "JsonUtilities",
			"Landscape", "LandscapeEditor", "Foliage",
			"ProceduralMeshComponent", "MeshDescription",
			"UMG",
			"Text3D",
			"LevelSequence"
		});

		PrivateDependencyModuleNames.AddRange(new string[] {  });

		// Uncomment if you are using Slate UI
		PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore", "MovieScene" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true

	}
}
