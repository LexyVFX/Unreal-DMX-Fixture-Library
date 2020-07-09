/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided 
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

using System;
using System.IO;

using UnrealBuildTool;

public class NDIIO : ModuleRules
{
	public NDIIO(ReadOnlyTargetRules Target) : base(Target)
	{        
        bEnforceIWYU = true;        
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        #region Public Includes

        // Include the Public include paths
        if (Directory.Exists(Path.Combine(ModuleDirectory, "Public")))
        {
            PublicIncludePaths.AddRange(new string[] {
                // ... add public include paths required here ...
                Path.Combine(ModuleDirectory, "Public" ),
            });
        }

        // Define the public dependencies
        PublicDependencyModuleNames.AddRange(new string[] {
            "Engine",
            "Core",
            "CoreUObject",
            "Projects"
        });

        #endregion

        #region Private Includes

        if (Directory.Exists(Path.Combine(ModuleDirectory, "Private")))
        {
            PrivateIncludePaths.AddRange(new string[] {
			    // ... add other private include paths required here ...
                Path.Combine(ModuleDirectory, "Private" )
            });
        }

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Renderer",
            "RenderCore",
            "RHI",
            "Slate",
            "SlateCore",
            "UMG",
            "ImageWrapper",
            "AudioMixer",
            "InputCore",

            "Media",
            "MediaAssets",
            "MediaUtils",

            "CinematicCamera"
        });

        #endregion

        #region Editor Includes

        if (Target.bBuildEditor == true)
        {
            PrivateIncludePathModuleNames.AddRange(new string[] {
                "AssetTools",
                "TargetPlatform",
            });

            PrivateDependencyModuleNames.AddRange(new string[] {
                "UnrealEd",
                "AssetTools",
                "MaterialUtilities"
            });
        }

        #endregion

        #region ThirdParty Includes

        // Our plugin only support the Win64 platform
        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            // Get the environment variables for the sdk and runtime for NDI
            string sdk_path = Path.GetFullPath(Path.Combine(ModuleDirectory, "ThirdParty", "NDI"));
            string runtime_path = Environment.GetEnvironmentVariable("NDI_RUNTIME_DIR_V4");

            // When building the project you must have the NDI SDK installed...
            // verify the sdk and runtime environment variables exist
            if (!string.IsNullOrEmpty(sdk_path) && !string.IsNullOrEmpty(runtime_path))
            {
                // Retrieve the directories from the environmental paths
                string SDK_DIRECTORY = Path.GetFullPath(sdk_path);
                string LIB_DIRECTORY = Path.GetFullPath(Path.Combine(sdk_path, "Libraries/Win64"));
                string RUNTIME_DIRECTORY = Path.GetFullPath(runtime_path);

                // Determine if the 'SDK' directory exists
                if (Directory.Exists(Path.Combine(SDK_DIRECTORY, "Includes")))
                {
                    PublicIncludePaths.AddRange(new string[] {
			            // ... add other private include paths required here ...
                        Path.Combine(SDK_DIRECTORY, "Includes"),
                    });
                }
                
                // ensure that we have both the sdk and runtime directories
                if (Directory.Exists(LIB_DIRECTORY) && Directory.Exists(RUNTIME_DIRECTORY))
                {
                    // Add the Library Files to a collection
                    String[] LibraryFilePaths = new string[] {
                        Path.Combine(LIB_DIRECTORY, "Processing.NDI.Lib.x64.lib"),
                    };

                    // Load the .lib files from the FilePaths collection
                    foreach (var FilePath in LibraryFilePaths)
                        PublicAdditionalLibraries.Add(FilePath);

                    // Load the DLL from the runtime directory
                    PublicDelayLoadDLLs.Add("Processing.NDI.Lib.x64.dll");
                    PublicRuntimeLibraryPaths.Add(RUNTIME_DIRECTORY);

                    // Ensure that we define our c++ define
                    PublicDefinitions.Add("NDI_SDK_ENABLED");
                }
            }
        }

        #endregion
    }
}


