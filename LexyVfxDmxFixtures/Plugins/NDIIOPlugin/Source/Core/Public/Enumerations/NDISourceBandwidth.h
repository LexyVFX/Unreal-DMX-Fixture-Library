/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#pragma once

#include <CoreMinimal.h>

#include "NDISourceBandwidth.generated.h"

/** 
	Receiver Bandwidth modes
*/
UENUM(BlueprintType, META = (DisplayName = "NDI Source Bandwidth"))
enum class ENDISourceBandwidth : uint8
{
	/** Receive metadata. */
	MetadataOnly	= 0x00 UMETA(DisplayName = "Metadata Only"),

	/** Receive metadata, audio */
	AudioOnly		= 0x01 UMETA(DisplayName = "Audio Only"),

	/** Receive metadata, audio, video at a lower bandwidth and resolution. */
	Lowest			= 0x02 UMETA(DisplayName = "Lowest"),

	// Receive metadata, audio, video at full resolution.
	Highest			= 0x03 UMETA(DisplayName = "Highest")
};