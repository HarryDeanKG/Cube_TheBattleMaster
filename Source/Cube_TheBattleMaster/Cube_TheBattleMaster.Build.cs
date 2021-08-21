// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Cube_TheBattleMaster : ModuleRules
{
	public Cube_TheBattleMaster(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay", "Niagara", "Json" , "GameplayTasks" });

        //PublicIncludePaths.AddRange(new string[] {
        //    Path.Combine(ModuleDirectory, "../../Engine/Plugins/FX/Niagara/Source/Niagara"),
        //    Path.Combine(ModuleDirectory, "Niagara")
        //});
    }
}
