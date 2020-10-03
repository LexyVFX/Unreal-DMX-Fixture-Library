/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#pragma once

#include <CoreMinimal.h>

#include <vector>
#include <algorithm>
#include <functional>
#include <chrono>

#include <Engine/World.h>
#include <Interfaces/IPluginManager.h>
#include <Modules/ModuleManager.h>

#include <NDIIOPluginSettings.h>

class NDIIO_API FNDIIOPluginModule : public IModuleInterface
{
	public:
		/** IModuleInterface implementation */
		virtual void StartupModule() override;
		virtual void ShutdownModule() override;

		bool BeginBroadcastingActiveViewport();
		void StopBroadcastingActiveViewport();		
		
	private:
		bool LoadModuleDependecies();		

	private:
		TSharedPtr<class FNDIFinderService> NDIFinderService = nullptr;
		TSharedPtr<class FNDIConnectionService> NDIConnectionService = nullptr;

		void* NDI_LIB_HANDLE = nullptr;
};