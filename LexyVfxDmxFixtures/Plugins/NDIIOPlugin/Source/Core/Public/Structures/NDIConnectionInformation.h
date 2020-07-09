/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#pragma once

#include <NDIIOPluginAPI.h>
#include <Enumerations/NDISourceBandwidth.h>
#include <Serialization/Archive.h>

#include "NDIConnectionInformation.generated.h"

/** 
	Describes essential properties used for connection objects over NDI®
*/
USTRUCT(BlueprintType, Blueprintable, Category = "NDI IO", META = (DisplayName = "NDI Connection Information"))
struct NDIIO_API FNDIConnectionInformation
{
	GENERATED_USTRUCT_BODY()

	public:
		/** A user-friendly name of the source */
		UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Properties", META = (DisplayName = "Source Name") )
		FString SourceName = FString("Black");

		/** The machine name of the source */
		UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Properties", META = (DisplayName = "Machine Name"))
		FString MachineName = FString("");

		/** The stream name of the source */
		UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Properties", META = (DisplayName = "Stream Name"))
		FString StreamName = FString("");

		/** A location on the network for which this source exists */
		UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Properties", META = (DisplayName = "Url"))
		FString Url = FString("");

		/** Indicates the current bandwidth mode used for this connection */
		UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Properties", META = (DisplayName = "Bandwidth"))
		ENDISourceBandwidth Bandwidth = ENDISourceBandwidth::Highest;

		/** Indicates whether the source that uses this object is current connected to another NDI® source */
		UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Properties", META = (DispalyName = "Is Currently Connected?"))
		bool bIsCurrentlyConnected = false;

	public:
		/** Constructs a new instance of this object */
		FNDIConnectionInformation() = default;
		
		/** Copies an existing instance to this object */
		FNDIConnectionInformation(const FNDIConnectionInformation& other);

		/** Copies existing instance properties to this object */
		FNDIConnectionInformation& operator =(const FNDIConnectionInformation& other);		

		/** Destructs this object */
		virtual ~FNDIConnectionInformation() = default;

		/** Implicit conversion to a base NDI receive create structure */
		operator NDIlib_recv_create_v3_t() const;		

		/** Implicit conversion to a base NDI source structure */
		operator NDIlib_source_t() const;

		/** Compares this object to 'other' and returns a determination of whether they are equal */
		bool operator ==(const FNDIConnectionInformation& other) const;

		/** Compares this object to 'other" and returns a determination of whether they are NOT equal */
		bool operator !=(const FNDIConnectionInformation& other) const;

	public:
		/** Resets the current parameters to the default property values */
		void Reset();
		
		/** Determines whether this object is valid connection information */
		bool IsValid() const;

	protected:
		/** Attempts to serialize this object using an Archive object */
		virtual FArchive& Serialize(FArchive& Ar);

	private:
		/** Operator override for serializing this object to an Archive object */
		friend class FArchive& operator <<(FArchive& Ar, FNDIConnectionInformation& Input) { return Input.Serialize(Ar); }
};