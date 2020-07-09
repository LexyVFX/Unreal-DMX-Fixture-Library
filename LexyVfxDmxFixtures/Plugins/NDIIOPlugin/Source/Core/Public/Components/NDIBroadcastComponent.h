/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#pragma once

#include <CoreMinimal.h>

#include <Components/ActorComponent.h>
#include <Objects/Media/NDIMediaSender.h>
#include <Structures/NDIBroadcastConfiguration.h>

#include "NDIBroadcastComponent.generated.h"

/** 
	Provides a wrapper to allow you to modify  an NDI Media Sender object from blueprints and perform broadcasting
	functionality
*/
UCLASS(BlueprintType, Blueprintable, Category = "NDI IO", META = (DisplayName = "NDI Broadcast Component", BlueprintSpawnableComponent))
class NDIIO_API UNDIBroadcastComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

	private:
		/** The NDI Media Sender representing the configuration of the network source to send audio, video, and metadata */
		UPROPERTY(EditDefaultsOnly, Category = "Properties", META = (DisplayName = "NDI Media Source", AllowPrivateAccess = true))
		UNDIMediaSender* NDIMediaSource = nullptr;

	public:
		/** 
			Initialize this component with the media source required for sending NDI audio, video, and metadata.
			Returns false, if the MediaSource is already been set. This is usually the case when this component is
			initialized in Blueprints.
		*/
		bool Initialize(UNDIMediaSender* InMediaSource = nullptr);

		/** 
			Attempts to start broadcasting audio, video, and metadata via the 'NDIMediaSource' associated with this object

			@param ErrorMessage The error message received when the media source is unable to start broadcasting
			@result Indicates whether this object successfully started broadcasting
		*/
		UFUNCTION(BlueprintCallable, Category = "NDI IO", META = (DisplayName = "Start Broadcasting"))
		bool StartBroadcasting(FString& ErrorMessage);

		/** 
			Changes the name of the sender object as seen on the network for remote connections

			@param InSourceName The new name of the source to be identified as on the network
		*/
		UFUNCTION(BlueprintCallable, Category = "NDI IO", META = (DisplayName = "Change Source Name"))
		void ChangeSourceName(const FString& InSourceName);

		/** 
			Attempts to change the Broadcast information associated with this media object

			@param InConfiguration The new configuration to broadcast
		*/
		UFUNCTION(BlueprintCallable, Category = "NDI IO", META = (DisplayName = "Change Broadcast Configuration"))
		void ChangeBroadcastConfiguration(const FNDIBroadcastConfiguration& InConfiguration);

		/**
			Attempts to change the RenderTarget used in sending video frames over NDI

			@param BroadcastTexture The texture to use as video, while broadcasting over NDI
		*/
		UFUNCTION(BlueprintCallable, Category = "NDI IO", META = (DisplayName = "Change Broadcast Texture"))
		void ChangeBroadcastTexture(UTextureRenderTarget2D* BroadcastTexture = nullptr);

		/**
			Determines the current tally information.

			@param IsOnPreview - A state indicating whether this source in on preview of a receiver
			@param IsOnProgram - A state indicating whether this source is on program of a receiver			
		*/
		UFUNCTION(BlueprintCallable, Category = "NDI IO", META = (DisplayName = "Get Tally Information"))
		void GetTallyInformation(bool& IsOnPreview, bool& IsOnProgram);

		/**
			Gets the current number of receivers connected to this source. This can be used to avoid rendering
			when nothing is connected to the video source. which can significantly improve the efficiency if 
			you want to make a lot of sources available on the network

			@param Result The total number of connected receivers attached to the broadcast of this object
		*/
		UFUNCTION(BlueprintCallable, Category = "NDI IO", META = (DisplayName = "Get Number of Connections"))
		void GetNumberOfConnections(int32& Result);

		/** 
			Attempts to immediately stop sending frames over NDI to any connected receivers 
		*/
		UFUNCTION(BlueprintCallable, Category = "NDI IO", META = (DisplayName = "Stop Broadcasting"))
		void StopBroadcasting();
};