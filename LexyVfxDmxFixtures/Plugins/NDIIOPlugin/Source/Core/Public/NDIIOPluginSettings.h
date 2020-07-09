/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#pragma once

#include <CoreMinimal.h>

#include <Misc/FrameRate.h>
#include <UObject/Object.h>

#include "NDIIOPluginSettings.generated.h"

/** 
	Settings for the Broadcasting of the Active Viewport configurable in the running editor of the application
*/
UCLASS(Config = Engine, DefaultConfig)
class NDIIO_API UNDIIOPluginSettings : public UObject
{
	GENERATED_BODY()

	public:
		UPROPERTY(VisibleAnywhere, Category = "NDI IO", META = (DisplayName = "Description", MultiLine = true))
		FString Decription = TEXT(
			"These values define the 'Active Viewport' broadcast settings and does not define default values for outputs." \
			"\r\n" \
			"\r\nApplication Stream Name - The default name to use when broadcasting the Currently Active Viewport over NDI." \
			"\r\nBroadcast Rate - Indicates the preferred frame rate to broadcast the Currently Active Viewport over NDI." \
			"\r\nPreferred FrameSize - Indicates the preferred frame size to broadcast the Currently Active Viewport over NDI."
		);

		/** The default name to use when broadcasting the Currently Active Viewport over NDI. */
		UPROPERTY(Config, EditAnywhere, Category = "NDI IO")
		FString ApplicationStreamName = FString("Unreal Engine");

		/** Indicates the preferred frame rate to broadcast the Currently Active Viewport over NDI. */
		UPROPERTY(Config, EditAnywhere, Category = "NDI IO", META = (DisplayName = "Broadcast Rate"))
		FFrameRate BroadcastRate = FFrameRate(60, 1);

		/** Indicates the preferred frame size to broadcast the Currently Active Viewport over NDI. */
		UPROPERTY(Config, EditAnywhere, Category = "NDI IO", META = (DisplayName = "Preferred Broadcast Framesize"))
		FIntPoint PreferredFrameSize = FIntPoint(1920, 1080);
};