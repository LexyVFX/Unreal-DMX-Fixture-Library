/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#pragma once

#include <CoreMinimal.h>

#include "NDIBroadcastConfiguration.generated.h"


/** 
	Describes essential properties used for modifying the broadcast configuration of an Sender object
*/
USTRUCT(BlueprintType, Blueprintable, Category = "NDI IO", META = (DisplayName = "NDI Broadcast Configuration"))
struct NDIIO_API FNDIBroadcastConfiguration
{
	GENERATED_USTRUCT_BODY()

	public:
		/** Describes the output frame size while sending video frame over NDI */
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Broadcast Settings", META = (DisplayName = "Frame Size"))
		FIntPoint FrameSize = FIntPoint(1920, 1080);

		/** Represents the desired number of frames (per second) for video to be sent over NDI */
		UPROPERTY(BlueprintReadwrite, EditAnywhere, Category = "Broadcast Settings", META = (DisplayName = "Frame Rate"))
		FFrameRate FrameRate = FFrameRate(60, 1);

	public:
		/** Constructs a new instance of this object */
		FNDIBroadcastConfiguration() = default;

		/** Copies an existing instance to this object */
		FNDIBroadcastConfiguration(const FNDIBroadcastConfiguration& other);

		/** Copies existing instance properties to this object */
		FNDIBroadcastConfiguration& operator =(const FNDIBroadcastConfiguration& other);

		/** Destructs this object */
		virtual ~FNDIBroadcastConfiguration() = default;

		/** Compares this object to 'other' and returns a determination of whether they are equal */
		bool operator ==(const FNDIBroadcastConfiguration& other) const;

		/** Compares this object to 'other" and returns a determination of whether they are NOT equal */
		bool operator !=(const FNDIBroadcastConfiguration& other) const;

	protected:
		/** Attempts to serialize this object using an Archive object */
		virtual FArchive& Serialize(FArchive& Ar);

	private:
		/** Operator override for serializing this object to an Archive object */
		friend class FArchive& operator <<(FArchive& Ar, FNDIBroadcastConfiguration& Input) { return Input.Serialize(Ar); }
};