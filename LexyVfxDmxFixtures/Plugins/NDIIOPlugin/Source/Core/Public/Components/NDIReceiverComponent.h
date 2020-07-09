/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#pragma once

#include <CoreMinimal.h>

#include <Components/ActorComponent.h>
#include <Structures/NDIConnectionInformation.h>
#include <Objects/Media/NDIMediaReceiver.h>

#include "NDIReceiverComponent.generated.h"

/** 
	A component used to receive audio, video, and metadata over NDI
*/
UCLASS(BlueprintType, Blueprintable, Category = "NDI IO", META = (DisplayName = "NDI Receiver Component", BlueprintSpawnableComponent))
class NDIIO_API UNDIReceiverComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

	private:
		/** The NDI Media Receiver representing the configuration of the network source to receive audio, video, and metadata from */
		UPROPERTY(EditDefaultsOnly, Category = "Properties", META = (DisplayName = "NDI Media Source", AllowPrivateAccess = true))
		UNDIMediaReceiver* NDIMediaSource = nullptr;

	public:
		/** 
			Initialize this component with the media source required for receiving NDI audio, video, and metadata.
			Returns false, if the MediaSource is already been set. This is usually the case when this component is
			initialized in Blueprints.
		*/
		bool Initialize(UNDIMediaReceiver* InMediaSource = nullptr);

		/** 
			Begin receiving NDI audio, video, and metadata frames 
		*/
		UFUNCTION(BlueprintCallable, Category = "NDI IO", META = (DisplayName = "Start Receiver"))
		bool StartReceiver(const FNDIConnectionInformation& InConnectionInformation);

		/** 
			Attempt to change the connection for which to get audio, video, and metadata frame from 
		*/
		UFUNCTION(BlueprintCallable, Category = "NDI IO", META = (DisplayName = "Change Connection"))
		void ChangeConnection(const FNDIConnectionInformation& InConnectionInformation);
		
		/**
			This will add a metadata frame and return immediately, having scheduled the frame asynchronously
		*/
		UFUNCTION(BlueprintCallable, Category = "NDI IO", META = (DisplayName = "Send Metadata Frame"))
		void SendMetadataFrame();

		/** 
			This will setup the up-stream tally notifications. If no streams are connected, it will automatically send 
			the tally state upon connection
		*/
		UFUNCTION(BlueprintCallable, Category = "NDI IO", META = (DisplayName = "Send Tally Information"))
		void SendTallyInformation(const bool& IsOnPreview, const bool& IsOnProgram);

		/** 
			Attempts to stop receiving audio, video, and metadata frame from the connected source 
		*/
		UFUNCTION(BlueprintCallable, Category = "NDI IO", META = (DisplayName = "Shutdown Receiver"))
		void ShutdownReceiver();

	public:
		/** 
			Returns the current framerate of the connected source
		*/
		UFUNCTION(BlueprintCallable, Category = "NDI IO", META = (DisplayName = "Get Current Frame Rate"))
		FFrameRate GetCurrentFrameRate() const;

		/** 
			Returns the current timecode of the connected source
		*/
		UFUNCTION(BlueprintCallable, Category = "NDI IO", META = (DisplayName = "Get Current Timecode"))
		FTimecode GetCurrentTimecode() const;

		/** 
			Returns the current connection information of the connected source 
		*/
		UFUNCTION(BlueprintCallable, Category = "NDI IO", META = (DisplayName = "Get Current Connection Information"))
		FNDIConnectionInformation GetCurrentConnectionInformation() const;

		/** 
			Returns the current performance data of the receiver while connected to the source
		*/
		UFUNCTION(BlueprintCallable, Category = "NDI IO", META = (DisplayName = "Get Performance Data"))
		FNDIReceiverPerformanceData GetPerformanceData() const;
};