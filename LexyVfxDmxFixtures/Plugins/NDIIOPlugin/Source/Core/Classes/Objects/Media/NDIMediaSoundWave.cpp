/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#include <Objects/Media/NDIMediaSoundWave.h>

UNDIMediaSoundWave::UNDIMediaSoundWave(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) { 

	// Set the Default Values for this object
	this->bProcedural = true;	
	this->bLooping = false;
	this->NumChannels = 1;
	this->SampleRate = 48000;

	this->SoundGroup = SOUNDGROUP_Default;
	this->Duration = INDEFINITELY_LOOPING_DURATION;	

	this->bOverrideConcurrency = true;
	this->ConcurrencyOverrides.bLimitToOwner = true;
	this->ConcurrencyOverrides.MaxCount = 2;
}

/**
	Set the Media Source of this object, so that when this object is called to 'GeneratePCMData' by the engine
	we can request the media source to provide the pcm data from the current connected source
*/
void UNDIMediaSoundWave::SetConnectionSource(UNDIMediaReceiver* InMediaSource) 
{ 
	// Ensure there is no thread contention for generating pcm data from the connection source
	FScopeLock Lock(&SyncContext);

	// Do we have a media source object to work with
	if (this->MediaSource != nullptr)
	{
		// Are we already registered with the incoming media source object
		if (this->MediaSource != InMediaSource)
		{
			// It doesn't look like we are registered with the incoming, make sure
			// to unregistered with the previous source
			this->MediaSource->UnregisterAudioWave(this);
		}	
	}

	// Ensure we have a reference to the media source object
	this->MediaSource = InMediaSource; 
}

/**
	Called by the engine to generate pcm data to be 'heard' by audio listener objects
*/
int32 UNDIMediaSoundWave::GeneratePCMData(uint8* PCMData, const int32 SamplesNeeded)
{
	// Ensure there is no thread contention for generating pcm data from the connection source
	FScopeLock Lock(&SyncContext);

	// set the default value, in case we have no connection source
	int32 samples_generated = SamplesNeeded * sizeof(int16);

	// check the connection source and continue
	if (this->MediaSource != nullptr)
	{
		// Ask the connection source to generate pcm data for this object
		samples_generated = MediaSource->GeneratePCMData(this, PCMData, SamplesNeeded);		
	}
	else
	{
		// just send silence since we don't have a connection source
		FMemory::Memset(PCMData, 0, samples_generated);
	}	

	// return to the engine the number of samples actually generated
	return samples_generated;
}

bool UNDIMediaSoundWave::IsReadyForFinishDestroy()
{
	// Ensure that there is no thread contention for generating data
	FScopeLock Lock(&SyncContext);

	// Since we have completed the wait... return we are ready for destroy
	return true;
}

void UNDIMediaSoundWave::InitAudioResource(FByteBulkData& CompressedData) { }

bool UNDIMediaSoundWave::InitAudioResource(FName Format) { return true; }

void UNDIMediaSoundWave::Serialize(FArchive& Ar) { return USoundWave::Serialize(Ar); }

bool UNDIMediaSoundWave::HasCompressedData(FName Format, ITargetPlatform* TargetPlatform) const { return false; }

int32 UNDIMediaSoundWave::GetResourceSizeForFormat(FName Format) { return 0; }

FByteBulkData* UNDIMediaSoundWave::GetCompressedData(FName Format, const FPlatformAudioCookOverrides* CompressionOverrides) { return nullptr; }
