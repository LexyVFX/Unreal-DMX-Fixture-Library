/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#include <Structures/NDIConnectionInformation.h>

/** Copies an existing instance to this object */
FNDIConnectionInformation::FNDIConnectionInformation(const FNDIConnectionInformation& other)
{
	// perform a deep copy of the 'other' structure and store the values in this object
	this->Bandwidth = other.Bandwidth;
	this->bIsCurrentlyConnected = other.bIsCurrentlyConnected;
	this->MachineName = other.MachineName;
	this->SourceName = other.SourceName;
	this->StreamName = other.StreamName;
	this->Url = other.Url;
}

/** Copies existing instance properties to this object */
FNDIConnectionInformation& FNDIConnectionInformation::operator =(const FNDIConnectionInformation& other)
{
	// perform a deep copy of the 'other' structure
	this->Bandwidth = other.Bandwidth;
	this->bIsCurrentlyConnected = other.bIsCurrentlyConnected;
	this->MachineName = other.MachineName;
	this->SourceName = other.SourceName;
	this->StreamName = other.StreamName;
	this->Url = other.Url;

	// return the result of the copy
	return *this;
}

/** Compares this object to 'other' and returns a determination of whether they are equal */
bool FNDIConnectionInformation::operator ==(const FNDIConnectionInformation& other) const 
{
	// return the value of a deep compare against the 'other' structure
	return this->Bandwidth == other.Bandwidth &&
		this->bIsCurrentlyConnected == other.bIsCurrentlyConnected &&
		this->MachineName == other.MachineName &&
		this->SourceName == other.SourceName &&
		this->StreamName == other.StreamName &&
		this->Url == other.Url;
}

/** Implicit conversion to a base NDI receive create structure */
FNDIConnectionInformation::operator NDIlib_recv_create_v3_t() const
{
	// We should always make the SDK perform the field alignments
	const bool& allow_video_fields = true;

	// Build the connection source structure from this objects properties
	NDIlib_source_t ndi_connection_source = NDIlib_source_t(TCHAR_TO_UTF8(*this->SourceName), TCHAR_TO_UTF8(*this->Url));

	// Always make the SDK perform the color conversion to a color space we can handle easily
	NDIlib_recv_color_format_e color_format = NDIlib_recv_color_format_UYVY_BGRA;

	// Convert this objects bandwidth mode to the SDK's version
	NDIlib_recv_bandwidth_e recv_bandwidth =
		this->Bandwidth == ENDISourceBandwidth::MetadataOnly ? NDIlib_recv_bandwidth_metadata_only :
		this->Bandwidth == ENDISourceBandwidth::AudioOnly ? NDIlib_recv_bandwidth_audio_only :
		this->Bandwidth == ENDISourceBandwidth::Lowest ? NDIlib_recv_bandwidth_lowest :
		NDIlib_recv_bandwidth_highest;

	// return the explicit construction of the receiver create object used by the SDK
	return NDIlib_recv_create_v3_t(ndi_connection_source, color_format, recv_bandwidth, allow_video_fields);
}

/** Implicit conversion to a base NDI source structure */
FNDIConnectionInformation::operator NDIlib_source_t() const 
{ 
	// return the explicit construction of an NDI source structure used by the SDK
	return NDIlib_source_t(TCHAR_TO_UTF8(*this->SourceName), TCHAR_TO_UTF8(*this->Url));
}

/** Resets the current parameters to the default property values */
void FNDIConnectionInformation::Reset()
{
	// Ensure we reset all the properties of this object to nominal default properties
	this->Bandwidth = ENDISourceBandwidth::Highest;
	this->bIsCurrentlyConnected = false;
	this->MachineName = FString("");
	this->SourceName = FString("Black");
	this->StreamName = FString("");
	this->Url = FString("");
}

/** Attempts to serialize this object using an Archive object */
FArchive& FNDIConnectionInformation::Serialize(FArchive& Ar)
{
	// we want to make sure that we are able to serialize this object, over many different version of this structure
	int32 current_version = 0;

	// serialize this structure
	return Ar << current_version << this->Bandwidth << this->bIsCurrentlyConnected <<
		this->MachineName << this->SourceName << this->StreamName << this->Url;
}

/** Determines whether this object is valid connection information */
bool FNDIConnectionInformation::IsValid() const { return !this->Url.IsEmpty(); }

/** Compares this object to 'other" and returns a determination of whether they are NOT equal */
bool FNDIConnectionInformation::operator !=(const FNDIConnectionInformation& other) const { return !(*this == other); }