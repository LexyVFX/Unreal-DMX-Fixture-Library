/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#pragma once

#include <CoreMinimal.h>
#include <Sound/SoundWave.h>

#include "NDIMediaSoundWave.generated.h"

/** 
	Defines a SoundWave object used by an NDI Media Receiver object for capturing audio from 
	a network source
*/
UCLASS(NotBlueprintable, Category = "NDI IO", META = (DisplayName = "NDI Media Sound Wave"))
class NDIIO_API UNDIMediaSoundWave : public USoundWave
{
	GENERATED_UCLASS_BODY()

	public:
		/** 
			Set the Media Source of this object, so that when this object is called to 'GeneratePCMData' by the engine
			we can request the media source to provide the pcm data from the current connected source
		*/
		void SetConnectionSource(class UNDIMediaReceiver* InMediaSource = nullptr);

	protected:
		/** 
			Called by the engine to generate pcm data to be 'heard' by audio listener objects
		*/
		virtual int32 GeneratePCMData(uint8* PCMData, const int32 SamplesNeeded) override;

	protected:
		virtual void InitAudioResource(FByteBulkData& CompressedData) override;
		virtual void Serialize(FArchive& Ar) override;

		virtual bool HasCompressedData(FName Format, ITargetPlatform* TargetPlatform) const override;
		virtual bool InitAudioResource(FName Format) override;
		virtual bool IsReadyForFinishDestroy() override;

		virtual int32 GetResourceSizeForFormat(FName Format) override;
		virtual FByteBulkData* GetCompressedData(FName Format, const FPlatformAudioCookOverrides* CompressionOverrides) override;

	private:		
		bool bIsPlaying = false;
		FCriticalSection SyncContext;		
		class UNDIMediaReceiver* MediaSource = nullptr;
};