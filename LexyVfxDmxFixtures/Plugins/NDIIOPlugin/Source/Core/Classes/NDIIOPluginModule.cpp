/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#include <NDIIOPluginModule.h>

#include <Modules/ModuleManager.h>
#include <NDIIOPluginAPI.h>
#include <Misc/Paths.h>

#include <GenericPlatform/GenericPlatformMisc.h>

#include <Services/NDIConnectionService.h>
#include <Services/NDIFinderService.h>

#include <Misc/MessageDialog.h>

// Meaning the plugin is being compiled with the editor
#if WITH_EDITOR
	
	#include "ThumbnailRendering/ThumbnailManager.h"
	#include "ThumbnailRendering/TextureThumbnailRenderer.h"

	#include <ISettingsModule.h>
	#include <Editor.h>

	#include <Objects/Media/NDIMediaTexture2D.h>

#endif

#define LOCTEXT_NAMESPACE "FNDIIOPluginModule"

void FNDIIOPluginModule::StartupModule()
{
	// Doubly Ensure that this handle is nullptr
	NDI_LIB_HANDLE = nullptr;

	if (LoadModuleDependecies())
	{
		#if UE_EDITOR

		if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
		{
			SettingsModule->RegisterSettings("Project", "Plugins", "NDI",
				LOCTEXT("NDISettingsName", "NewTek NDI"),
				LOCTEXT("NDISettingsDescription", "NewTek NDI® Engine Intergration Settings"),
				GetMutableDefault< UNDIIOPluginSettings >()
			);			
		}

		// Ensure that the thumbnail for the 'NDI Media Texture2D' is being updated, as the texture is being used.
		UThumbnailManager::Get().RegisterCustomRenderer(
			UNDIMediaTexture2D::StaticClass(), 
			UTextureThumbnailRenderer::StaticClass()
		);

		#endif

		// Construct out Services
		this->NDIFinderService = MakeShareable(new FNDIFinderService());
		this->NDIConnectionService = MakeShareable(new FNDIConnectionService());

		// Start the service
		if (NDIFinderService.IsValid())
			NDIFinderService->Start();

		// Start the service
		if (NDIConnectionService.IsValid())
			NDIConnectionService->Start();
	}
	else
	{	
		// Write an error message to the log.
		UE_LOG(LogWindows, Error, TEXT("Unable to load \"Processing.NDI.Lib.x64.dll\" from the NDI 4 Runtime Directory.") );

		#if UE_EDITOR		

		const FText& WarningMessage = 
			LOCTEXT("NDIRuntimeMissing", 				
				"Cannot find \"Processing.NDI.Lib.x64.dll\" from the NDI 4 Runtime Directory. "\
				"Continued usage of the plugin can cause instability within the editor.\r\n\r\n"\
				
				"Please refer to the 'NDI IO Plugin for Unreal Engine Quickstart Guide' "\
				"for additional information related to installation instructions for this plugin.\r\n\r\n"				
			);

		// Open a message box, showing that things will not work since the NDI Runtime Directory cannot be found
		if (FMessageDialog::Open(EAppMsgType::OkCancel, EAppReturnType::Ok, WarningMessage) == EAppReturnType::Ok)
		{
			FString URLResult = FString("");
			FPlatformProcess::LaunchURL(*FString("http://new.tk/UnrealNDISDK425Guide"), nullptr, &URLResult);
		}

		#endif
	}
}

void FNDIIOPluginModule::ShutdownModule()
{
	if (NDIFinderService.IsValid())
		NDIFinderService->Shutdown();

	#if UE_EDITOR

	if (NDI_LIB_HANDLE != nullptr)
	{
		FPlatformProcess::FreeDllHandle(NDI_LIB_HANDLE);
		NDI_LIB_HANDLE = nullptr;
	}

	#endif
}

bool FNDIIOPluginModule::BeginBroadcastingActiveViewport()
{
	// Ensure we have a valid service
	if (NDIConnectionService.IsValid())
	{
		// perform the requested functionality
		return NDIConnectionService->BeginBroadcastingActiveViewport();
	}

	return false;
}

void FNDIIOPluginModule::StopBroadcastingActiveViewport()
{
	// Ensure we have a valid service
	if (NDIConnectionService.IsValid())
	{
		// perform the requested functionality
		NDIConnectionService->StopBroadcastingActiveViewport();
	}
}

bool FNDIIOPluginModule::LoadModuleDependecies()
{
	// Get the Binaries File Location
	const FString env_variable = TEXT(NDILIB_REDIST_FOLDER);
	const FString binaries_path = FPlatformMisc::GetEnvironmentVariable(*env_variable) + "/Processing.NDI.Lib.x64.dll";

	// We can't validate if it's valid, but we can determine if it's explicitly not.
	if (binaries_path.Len() > 0)
	{
		// Load the DLL
		this->NDI_LIB_HANDLE = FPlatformProcess::GetDllHandle(*binaries_path);

		// Not required, but "correct" (see the SDK documentation)
		if (NDI_LIB_HANDLE != nullptr && !NDIlib_initialize())
		{
			// We were unable to initialize the library, so lets free the handle
			FPlatformProcess::FreeDllHandle(NDI_LIB_HANDLE);
			NDI_LIB_HANDLE = nullptr;
		}
	}

	// Did we successfully load the NDI library?
	return NDI_LIB_HANDLE != nullptr;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FNDIIOPluginModule, NDIIO);