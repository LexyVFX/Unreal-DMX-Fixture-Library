/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#pragma once

#include <NDIIOPluginAPI.h>

#include <RendererInterface.h>
#include <UObject/Object.h>
#include <Misc/Framerate.h>
#include <Engine/TextureRenderTarget2D.h>
#include <Structures/NDIBroadcastConfiguration.h>
#include <Objects/Media/NDIMediaTexture2D.h>
#include <BaseMediaSource.h>

#include "NDIMediaSender.generated.h"

/** 
	A delegate used for notifications on property changes on the NDIMediaSender object
*/
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNDIMediaSenderPropertyChanged, UNDIMediaSender*, Sender);

/** 
	Defines a media object representing an NDI® Sender object. This object is used with the 
	NDI Broadcast Component to send Audio / Video / Metadata to a 'receiving' NDI object.
*/
UCLASS(BlueprintType, Blueprintable, HideCategories = ("Platforms"), Category = "NDI IO", META = (DisplayName = "NDI Sender Object"))
class NDIIO_API UNDIMediaSender : public UBaseMediaSource
{
	GENERATED_UCLASS_BODY()

	private:
		/** Describes a user-friendly name of the output stream to differentiate from other output streams on the current machine */
		UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Broadcast Settings", META = (DisplayName = "Source Name", AllowPrivateAccess = true))
		FString SourceName = TEXT("Unreal Engine Output");

		/** Describes the output frame size while sending video frame over NDI */
		UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Broadcast Settings", META = (DisplayName = "Frame Size", AllowPrivateAccess = true))
		FIntPoint FrameSize = FIntPoint(1920, 1080);

		/** Represents the desired number of frames (per second) for video to be sent over NDI */
		UPROPERTY(BlueprintReadwrite, EditDefaultsOnly, Category = "Broadcast Settings", META = (DisplayName = "Frame Rate", AllowPrivateAccess = true))
		FFrameRate FrameRate = FFrameRate(60, 1);

		/** Indicates the texture to send over NDI */
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Content", META = (DisplayName = "Render Target", AllowPrivateAccess = true))
		UTextureRenderTarget2D* RenderTarget = nullptr;

		/** 
			Should perform the sRGB to Linear color space conversion
		*/
		UPROPERTY(BlueprintReadonly, VisibleAnywhere, Category = "Information", META = (DisplayName = "Perform sRGB to Linear?", AllowPrivateAccess = true))
		bool bPerformsRGBtoLinear = true;

	public:
		UPROPERTY()
		FNDIMediaSenderPropertyChanged OnBroadcastConfigurationChanged;

	public:
		/** 
			Attempts to perform initialization logic for creating a sender through the NDI® sdk api 
		*/
		void Initialize();

		/** 
			Changes the name of the sender object as seen on the network for remote connections
		*/
		void ChangeSourceName(const FString& InSourceName);

		/** 
			Attempts to change the Broadcast information associated with this media object
		*/
		void ChangeBroadcastConfiguration(const FNDIBroadcastConfiguration& InConfiguration);

		/**
			This will add a metadata frame and return immediately, having schedule the frame asynchronously.
		*/
		void SendMetadataFrame();

		/** 
			Attempts to change the RenderTarget used in sending video frames over NDI
		*/
		void ChangeVideoTexture(UTextureRenderTarget2D* VideoTexture = nullptr);

		/**
			Determines the current tally information. If you specify a timeout then it will wait until it has
			changed, otherwise it will simply poll it and return the current tally immediately

			@param IsOnPreview - A state indicating whether this source in on preview of a receiver
			@param IsOnProgram - A state indicating whether this source is on program of a receiver
			@param TimeOut	- Indicates the amount of time to wait (in milliseconds) until a change has occurred
		*/
		void GetTallyInformation(bool& IsOnPreview, bool& IsOnProgram, uint32 Timeout = 0);

		/**
			Gets the current number of receivers connected to this source. This can be used to avoid rendering
			when nothing is connected to the video source. which can significantly improve the efficiency if 
			you want to make a lot of sources available on the network
		*/
		void GetNumberOfConnections(int32& Result);

		/** 
			Attempts to immediately stop sending frames over NDI to any connected receivers 
		*/
		void Shutdown();
			
		/**
		   Called before destroying the object.  This is called immediately upon deciding to destroy the object, 
		   to allow the object to begin an asynchronous cleanup process.
		 */
		virtual void BeginDestroy() override;

		/** 
			Set whether or not a RGB to Linear conversion is made
		*/
		void PerformsRGBToLinearConversion(bool Value);		

		/**
			Returns the Render Target used for sending a frame over NDI
		*/
		UTextureRenderTarget2D* GetRenderTarget();

		const FIntPoint& GetFrameSize() { return this->FrameSize; }

		const FFrameRate& GetFrameRate() { return this->FrameRate; }

	private:
		/**
			This will attempt to generate an audio frame, add the frame to the stack and return immediately, 
			having scheduled the frame asynchronously.
		*/
		void TrySendAudioFrame(int64 time_code = 0);
		
		/**
			This will attempt to generate a video frame, add the frame to the stack and return immediately, 
			having scheduled the frame asynchronously.
		*/
		void TrySendVideoFrame(int64 time_code = 0);
		
		/** 
			Perform the color conversion (if any) and bit copy from the gpu
		*/
		bool DrawRenderTarget(FRHICommandListImmediate& RHICmdList, void*& Result);

		/** 
			Change the render target configuration based on the passed in parameters

			@param InFrameSize The frame size to resize the render target to
			@param InFrameRate The frame rate at which we should be sending frames via NDI
		*/
		void ChangeRenderTargetConfiguration(FIntPoint InFrameSize, FFrameRate InFrameRate);

		virtual bool Validate() const override { return true; }
		virtual FString GetUrl() const override { return FString(); }

	private:		
		bool bIsChangingBroadcastSize = false;

		FTimecode LastRenderTime;

		NDIlib_video_frame_v2_t NDI_video_frame;
		NDIlib_send_instance_t p_send_instance = nullptr;

		FCriticalSection AudioSyncContext;
		FCriticalSection RenderSyncContext;

		FTexture2DRHIRef ReadbackTexture = nullptr;
		FPooledRenderTargetDesc RenderTargetDescriptor;
};