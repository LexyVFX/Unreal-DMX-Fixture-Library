/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#pragma once

#include <NDIIOPluginAPI.h>

#include <UObject/Object.h>
#include <Misc/Timecode.h>
#include <Misc/FrameRate.h>

#include <Objects/Media/NDIMediaSoundWave.h>
#include <Objects/Media/NDIMediaTexture2D.h>
#include <Structures/NDIConnectionInformation.h>
#include <Structures/NDIReceiverPerformanceData.h>

#include "NDIMediaReceiver.generated.h"

/** 
	A Media object representing the NDI Receiver for being able to receive Audio, Video, and Metadata over NDI®
*/
UCLASS(BlueprintType, Blueprintable, Category = "NDI IO", HideCategories = ("Information"), META = (DisplayName = "NDI Media Receiver"))
class NDIIO_API UNDIMediaReceiver : public UObject
{
	GENERATED_UCLASS_BODY()

	private:
		/** 
			The current frame count, seconds, minutes, and hours in time-code notation
		*/
		UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Information", META = (DisplayName = "Timecode", AllowPrivateAccess = true))
		FTimecode Timecode;

		/** 
			The desired number of frames (per second) for video to be displayed
		*/
		UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Information", META = (DisplayName = "Frame Rate", AllowPrivateAccess = true))
		FFrameRate FrameRate;

		/** 
			Should perform the sRGB to Linear color space conversion
		*/
		UPROPERTY(BlueprintReadonly, VisibleAnywhere, Category = "Information", META = (DisplayName = "Perform sRGB to Linear?", AllowPrivateAccess = true))
		bool bPerformsRGBtoLinear = true;

		/** 
			Information describing detailed information about the sender this receiver is connected to
		*/
		UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Information", META = (DisplayName = "Connection Information", AllowPrivateAccess = true))
		FNDIConnectionInformation ConnectionInformation;

		/** 
			Information describing detailed information about the receiver performance when connected to an NDI® sender
		*/
		UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Information", META = (DisplayName = "Performance Data", AllowPrivateAccess = true))
		FNDIReceiverPerformanceData PerformanceData;

		/** 
			Provides an NDI Sound Wave object to render audio frames from the source into.
		*/
		UPROPERTY(BlueprintReadWrite, EditAnywhere, BlueprintSetter = "ChangeAudioWave", Category = "Content", META = (DisplayName = "Audio Wave", AllowPrivateAccess = true))
		UNDIMediaSoundWave* AudioWave = nullptr;

		/** 
			Provides an NDI Video Texture object to render videos frames from the source onto 
		*/
		UPROPERTY(BlueprintReadWrite, EditAnywhere, BlueprintSetter = "ChangeVideoTexture", Category = "Content", META = (DisplayName = "Video Texture", AllowPrivateAccess = true))
		UNDIMediaTexture2D* VideoTexture = nullptr;

	public:
		/** 
			Attempts to perform initialization logic for creating a receiver through the NDI® sdk api 
		*/
		bool Initialize(const FNDIConnectionInformation& InConnectionInformation);

		/** 
			Attempts to change the connection to another NDI® sender source
		*/
		void ChangeConnection(const FNDIConnectionInformation& InConnectionInformation);
		
		/** 
			Attempts to change the SoundWave object used as the audio frame capture object
		*/
		UFUNCTION(BlueprintSetter)
		void ChangeAudioWave(UNDIMediaSoundWave* InAudioWave = nullptr);

		/** 
			Attempts to change the Video Texture object used as the video frame capture object
		*/
		UFUNCTION(BlueprintSetter)
		void ChangeVideoTexture(UNDIMediaTexture2D* InVideoTexture = nullptr);

		/** 
			Attempts to generate the pcm data required by the 'AudioWave' object
		*/
		int32 GeneratePCMData(uint8* PCMData, const int32 SamplesNeeded);

		/** 
			This will add a metadata frame and return immediately, having scheduled the frame asynchronously
		*/
		void SendMetadataFrame();

		/** 
			This will set the up-stream tally notifications. If no streams are connected, it will automatically 
			send the tally state upon connection
		*/
		void SendTallyInformation(const bool& IsOnPreview, const bool& IsOnProgram);

		/** 
			Attempts to immediately stop receiving frames from the connected NDI sender 
		*/
		void Shutdown();

		/**
		   Called before destroying the object.  This is called immediately upon deciding to destroy the object, 
		   to allow the object to begin an asynchronous cleanup process.
		 */
		void BeginDestroy() override;

	private:
		/**
			Attempts to immediately update the 'VideoTexture' object with the last capture video frame
			from the connected source
		*/
		void CaptureConnectedVideo();

		/** 
			Attempts to gather the performance metrics of the connection to the remote source 
		*/
		void GatherPerformanceMetrics();

	public:
		/** 
			Set whether or not a RGB to Linear conversion is made
		*/
		void PerformsRGBToLinearConversion(bool Value);

		/** 
			Returns the current framerate of the connected source
		*/		
		const FFrameRate& GetCurrentFrameRate() const;

		/** 
			Returns the current timecode of the connected source
		*/		
		const FTimecode& GetCurrentTimecode() const;

		/** 
			Returns the current connection information of the connected source 
		*/		
		const FNDIConnectionInformation& GetCurrentConnectionInformation() const;

		/** 
			Returns the current performance data of the receiver while connected to the source
		*/		
		const FNDIReceiverPerformanceData& GetPerformanceData() const;

		/** 
			Returns the sound wave object used by this object for audio
		*/
		UNDIMediaSoundWave* GetMediaSoundWave() const;

	private:
		/**
			Perform the color conversion (if any) and bit copy from the gpu
		*/
		void DrawVideoFrame(FRHICommandListImmediate& RHICmdList, NDIlib_video_frame_v2_t& Result);

	private:
		NDIlib_recv_instance_t p_receive_instance = nullptr;
		NDIlib_framesync_instance_t p_framesync_instance = nullptr;

		FCriticalSection AudioSyncContext;
		FCriticalSection RenderSyncContext;

		FTexture2DRHIRef SourceTexture;
		FTexture2DRHIRef ConversionTexture;
		FPooledRenderTargetDesc RenderTargetDescriptor;
};