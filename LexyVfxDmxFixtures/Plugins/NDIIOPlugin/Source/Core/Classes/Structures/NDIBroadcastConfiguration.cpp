/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#include <Structures/NDIBroadcastConfiguration.h>

/** Copies an existing instance to this object */
FNDIBroadcastConfiguration::FNDIBroadcastConfiguration(const FNDIBroadcastConfiguration& other)
{
	// perform a deep copy of the 'other' structure and store the values in this object
	this->FrameRate = other.FrameRate;
	this->FrameSize = other.FrameSize;
}

/** Copies existing instance properties to this object */
FNDIBroadcastConfiguration& FNDIBroadcastConfiguration::operator =(const FNDIBroadcastConfiguration& other)
{
	// perform a deep copy of the 'other' structure
	this->FrameRate = other.FrameRate;
	this->FrameSize = other.FrameSize;

	// return the result of the copy
	return *this;
}

/** Compares this object to 'other' and returns a determination of whether they are equal */
bool FNDIBroadcastConfiguration::operator ==(const FNDIBroadcastConfiguration& other) const
{
	// return the value of a deep compare against the 'other' structure
	return this->FrameRate == other.FrameRate &&
		this->FrameSize == other.FrameSize;
}

/** Attempts to serialize this object using an Archive object */
FArchive& FNDIBroadcastConfiguration::Serialize(FArchive& Ar)
{
	// we want to make sure that we are able to serialize this object, over many different version of this structure
	int32 current_version = 0;
	
	// serialize this structure
	return Ar << current_version << this->FrameRate.Numerator << this->FrameRate.Denominator << this->FrameSize;	
}

/** Compares this object to 'other" and returns a determination of whether they are NOT equal */
bool FNDIBroadcastConfiguration::operator !=(const FNDIBroadcastConfiguration& other) const { return !(*this == other); }