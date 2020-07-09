/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#pragma once

#include <NDIIOPluginAPI.h>
#include <Serialization/Archive.h>

#include "NDIReceiverPerformanceData.generated.h"

/** 
	A structure holding data allowing you to determine the current performance levels of the receiver with the 
	ability to detect whether frames has been dropped
*/
USTRUCT(BlueprintType, Blueprintable, Category = "NDI IO", META = (DisplayName = "NDI Receiver Performance Data"))
struct NDIIO_API FNDIReceiverPerformanceData
{
	GENERATED_USTRUCT_BODY()

	public:		
		/** 
			The number of audio frames received from the NDI sender
		*/
		UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Information", META = (DisplayName = "Audio Frames"))
		int64 AudioFrames = 0;
			
		/** 
			The number of video frames dropped in transit from an NDI sender
		*/
		UPROPERTY(BlueprintReadonly, VisibleAnywhere, Category = "Information", META = (DisplayName = "Dropped Video Frames"))
		int64 DroppedVideoFrames = 0;

		/** 
			The number of audio frames dropped in transit from the NDI sender
		*/
		UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Information", META = (DisplayName = "Dropped Audio Frames"))
		int64 DroppedAudioFrames = 0;

		/** 
			The number of metadata frames dropped in transit from the NDI sender
		*/
		UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Information", META = (DisplayName = "Dropped Metadata Frames"))
		int64 DroppedMetadataFrames = 0;
	
		/** 
			The number of metadata frames received from the NDI sender
		*/
		UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Information", META = (DisplayName = "Metadata Frames"))
		int64 MetadataFrames = 0;

		/** 
			The number of video frames received from the NDI sender
		*/
		UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Information", META = (DisplayName = "Video Frames"))
		int64 VideoFrames = 0;

	public:
		/** Constructs a new instance of this object */
		FNDIReceiverPerformanceData() = default;

		/** Copies an existing instance to this object */
		FNDIReceiverPerformanceData(const FNDIReceiverPerformanceData& other);

		/** Copies existing instance properties to this object */
		FNDIReceiverPerformanceData& operator =(const FNDIReceiverPerformanceData& other);

		/** Destructs this object */
		virtual ~FNDIReceiverPerformanceData() = default;
	
		/** Compares this object to 'other' and returns a determination of whether they are equal */
		bool operator ==(const FNDIReceiverPerformanceData& other) const;

		/** Compares this object to 'other" and returns a determination of whether they are NOT equal */
		bool operator !=(const FNDIReceiverPerformanceData& other) const;

	public:
		/** Resets the current parameters to the default property values */
		void Reset();

	protected:
		/** Attempts to serialize this object using an Archive object */
		virtual FArchive& Serialize(FArchive& Ar);

	private:
		/** Operator override for serializing this object to an Archive object */
		friend class FArchive& operator <<(FArchive& Ar, FNDIReceiverPerformanceData& Input) { return Input.Serialize(Ar); }
};