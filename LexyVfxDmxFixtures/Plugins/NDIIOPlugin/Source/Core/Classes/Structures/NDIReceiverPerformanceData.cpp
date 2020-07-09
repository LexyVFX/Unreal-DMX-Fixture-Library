/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#include <Structures/NDIReceiverPerformanceData.h>

/** Copies an existing instance to this object */
FNDIReceiverPerformanceData::FNDIReceiverPerformanceData(const FNDIReceiverPerformanceData& other)
{
	// perform a deep copy of the 'other' structure and store the values in this object
	this->AudioFrames = other.AudioFrames;
	this->DroppedAudioFrames = other.DroppedAudioFrames;
	this->DroppedMetadataFrames = other.DroppedMetadataFrames;
	this->DroppedVideoFrames = other.DroppedVideoFrames;
	this->MetadataFrames = other.MetadataFrames;
	this->VideoFrames = other.VideoFrames;
}

/** Copies existing instance properties to this object */
FNDIReceiverPerformanceData& FNDIReceiverPerformanceData::operator =(const FNDIReceiverPerformanceData& other)
{
	// perform a deep copy of the 'other' structure
	this->AudioFrames = other.AudioFrames;
	this->DroppedAudioFrames = other.DroppedAudioFrames;
	this->DroppedMetadataFrames = other.DroppedMetadataFrames;
	this->DroppedVideoFrames = other.DroppedVideoFrames;
	this->MetadataFrames = other.MetadataFrames;
	this->VideoFrames = other.VideoFrames;

	// return the result of the copy
	return *this;
}

/** Compares this object to 'other' and returns a determination of whether they are equal */
bool FNDIReceiverPerformanceData::operator ==(const FNDIReceiverPerformanceData& other) const
{
	// return the value of a deep compare against the 'other' structure
	return this->AudioFrames == other.AudioFrames &&
		this->DroppedAudioFrames == other.DroppedAudioFrames &&
		this->DroppedMetadataFrames == other.DroppedMetadataFrames &&
		this->DroppedVideoFrames == other.DroppedVideoFrames &&
		this->MetadataFrames == other.MetadataFrames &&
		this->VideoFrames == other.VideoFrames;
}

/** Resets the current parameters to the default property values */
void FNDIReceiverPerformanceData::Reset()
{
	// Ensure we reset all the properties of this object to nominal default properties
	this->AudioFrames = 0;
	this->DroppedAudioFrames = 0;
	this->DroppedMetadataFrames = 0;
	this->DroppedVideoFrames = 0;
	this->MetadataFrames = 0;
	this->VideoFrames = 0;
}

/** Attempts to serialize this object using an Archive object */
FArchive& FNDIReceiverPerformanceData::Serialize(FArchive& Ar)
{
	// we want to make sure that we are able to serialize this object, over many different version of this structure
	int32 current_version = 0;

	// serialize this structure
	return Ar << current_version << this->AudioFrames << this->DroppedAudioFrames << this->DroppedMetadataFrames <<
		 this->DroppedVideoFrames << this->MetadataFrames << this->VideoFrames;
}

/** Compares this object to 'other" and returns a determination of whether they are NOT equal */
bool FNDIReceiverPerformanceData::operator !=(const FNDIReceiverPerformanceData& other) const { return !(*this == other); }