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

#if PLATFORM_WINDOWS
	#include <Windows/AllowWindowsPlatformTypes.h>
	
	#ifndef NDI_SDK_ENABLED
		#error NDI® 4.0 SDK must be installed for the NDI® IO plugin to run properly.
	#endif

	#ifdef NDI_SDK_ENABLED
		#include <Processing.NDI.Lib.h>
		#include <Processing.NDI.Lib.cplusplus.h>
	#endif

	#include <Windows/HideWindowsPlatformTypes.h>	
#endif

#define NDIIO_MODULE_NAME FName(TEXT("NDIIO"))