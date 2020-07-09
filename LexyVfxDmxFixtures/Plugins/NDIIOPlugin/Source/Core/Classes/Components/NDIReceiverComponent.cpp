/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#include <Components/NDIReceiverComponent.h>

UNDIReceiverComponent::UNDIReceiverComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {


}

/**
	Initialize this component with the media source required for receiving NDI audio, video, and metadata.
	Returns false, if the MediaSource is already been set. This is usually the case when this component is
	initialized in Blueprints.
*/
bool UNDIReceiverComponent::Initialize(UNDIMediaReceiver* InMediaSource)
{
	if (this->IsOwnerRunningUserConstructionScript())
	{
		if (this->NDIMediaSource == nullptr && InMediaSource != nullptr)
		{
			this->NDIMediaSource = InMediaSource;
		}
	}

	return InMediaSource != nullptr && InMediaSource == NDIMediaSource;
}

/**
	Begin receiving NDI audio, video, and metadata frames
*/
bool UNDIReceiverComponent::StartReceiver(const FNDIConnectionInformation& InConnectionInformation)
{
	if (IsValid(this->NDIMediaSource))
	{
		// Call to the Media Source's function to initialize (hopefully with valid connection information)
		if (NDIMediaSource->Initialize(InConnectionInformation))
		{
			// FNDIConnectionService::RegisterReceiver(this->NDIMediaSource);
			return true;
		}
	}

	return false;
}

/**
	Attempt to change the connection for which to get audio, video, and metadata frame from
*/
void UNDIReceiverComponent::ChangeConnection(const FNDIConnectionInformation& InConnectionInformation)
{
	// Ensure a valid source to change the connection on
	if (IsValid(this->NDIMediaSource))
	{
		// Call the underlying function
		NDIMediaSource->ChangeConnection(InConnectionInformation);
	}
}

/**
	This will add a metadata frame and return immediately, having scheduled the frame asynchronously
*/
void UNDIReceiverComponent::SendMetadataFrame()
{
	// Ensure a valid source to send metadata from
	if (IsValid(this->NDIMediaSource))
	{
		// Call the underlying function
		NDIMediaSource->SendMetadataFrame();
	}
}

/**
	This will setup the up-stream tally notifications. If no streams are connected, it will automatically send
	the tally state upon connection
*/
void UNDIReceiverComponent::SendTallyInformation(const bool& IsOnPreview, const bool& IsOnProgram)
{
	if (IsValid(this->NDIMediaSource))
	{
		NDIMediaSource->SendTallyInformation(IsOnPreview, IsOnProgram);
	}
}

/**
	Attempts to stop receiving audio, video, and metadata frame from the connected source
*/
void UNDIReceiverComponent::ShutdownReceiver()
{
	if (IsValid(this->NDIMediaSource))
	{
		NDIMediaSource->Shutdown();
	}
}

/**
	Returns the current framerate of the connected source
*/
FFrameRate UNDIReceiverComponent::GetCurrentFrameRate() const  
{ 
	return IsValid(NDIMediaSource) ? 
		NDIMediaSource->GetCurrentFrameRate() : 
		FFrameRate(60, 1); 
}

/**
	Returns the current timecode of the connected source
*/
FTimecode UNDIReceiverComponent::GetCurrentTimecode() const
{
	return IsValid(NDIMediaSource) ? 
		NDIMediaSource->GetCurrentTimecode() : 
		FTimecode::FromTimespan( FTimespan::FromMilliseconds(0.0), FFrameRate(60, 1), false, true );
}

/**
	Returns the current connection information of the connected source
*/
FNDIConnectionInformation UNDIReceiverComponent::GetCurrentConnectionInformation() const
{
	return IsValid(NDIMediaSource) ?
		NDIMediaSource->GetCurrentConnectionInformation() :
		FNDIConnectionInformation();
}

/**
	Returns the current performance data of the receiver while connected to the source
*/
FNDIReceiverPerformanceData UNDIReceiverComponent::GetPerformanceData() const
{
	return IsValid(NDIMediaSource) ?
		NDIMediaSource->GetPerformanceData() :
		FNDIReceiverPerformanceData();
}