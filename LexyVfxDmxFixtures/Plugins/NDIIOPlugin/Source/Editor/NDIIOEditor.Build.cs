/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided 
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

using System;
using System.IO;

using UnrealBuildTool;

public class NDIIOEditor : ModuleRules
{
	public NDIIOEditor(ReadOnlyTargetRules Target) : base(Target)
	{
        bEnforceIWYU = true;
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        #region Public Includes

        if (Directory.Exists(Path.Combine(ModuleDirectory, "Public")))
        {
            PublicIncludePaths.AddRange(new string[] {
                // ... add public include paths required here ...
                Path.Combine(ModuleDirectory, "Public" ),
            });
        }

        PublicDependencyModuleNames.AddRange(new string[] {
            "Engine",
            "Core",
            "CoreUObject"
        });

        #endregion

        if (Target.bBuildEditor == true)
        {
            #region Private Includes

            if (Directory.Exists(Path.Combine(ModuleDirectory, "Private")))
            {
                PrivateIncludePaths.AddRange(new string[] {
			        // ... add other private include paths required here ...
                    Path.Combine(ModuleDirectory, "Private" ),
                    Path.Combine(ModuleDirectory, "../Core/Private"),
                    Path.Combine(ModuleDirectory, "../ThirdParty/NDI/Source"),
                });
            }

            #endregion

            PrivateIncludePathModuleNames.AddRange(new string[] {
                "AssetTools",
                "TargetPlatform",
            });

            PrivateDependencyModuleNames.AddRange(new string[] {
                "Projects",
                "UnrealEd",
                "AssetTools",
                "MaterialUtilities",
                "Renderer",
                "RenderCore",
                "PlacementMode",
                "CinematicCamera",

                "RHI",
                "Slate",
                "SlateCore",
                "UMG",
                "ImageWrapper",

                "Media",
                "MediaAssets",
                "MediaUtils",

                "AssetTools",
                "TargetPlatform",
                "PropertyEditor",

                "NDIIO"
            });
        }
    }
}
