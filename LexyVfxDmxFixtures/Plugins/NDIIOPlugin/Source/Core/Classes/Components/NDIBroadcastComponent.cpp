/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#include <Components/NDIBroadcastComponent.h>

UNDIBroadcastComponent::UNDIBroadcastComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {


}

/**
	Initialize this component with the media source required for sending NDI audio, video, and metadata.
	Returns false, if the MediaSource is already been set. This is usually the case when this component is
	initialized in Blueprints.
*/
bool UNDIBroadcastComponent::Initialize(UNDIMediaSender* InMediaSource)
{
	// we only allow initialization of the media source in the construction script
	// to prevent changes during tick-able moments
	if (this->IsOwnerRunningUserConstructionScript())
	{
		// is the media source already set?
		if (this->NDIMediaSource == nullptr && InMediaSource != nullptr)
		{
			// we passed validation, so set the media source
			this->NDIMediaSource = InMediaSource;
		}
	}

	// did we pass validation
	return InMediaSource != nullptr && InMediaSource == NDIMediaSource;
}

/**
	Attempts to start broadcasting audio, video, and metadata via the 'NDIMediaSource' associated with this object

	@param ErrorMessage The error message received when the media source is unable to start broadcasting
	@result Indicates whether this object successfully started broadcasting
*/
bool UNDIBroadcastComponent::StartBroadcasting(FString& ErrorMessage)
{
	// validate the Media Source object
	if (IsValid(NDIMediaSource))
	{
		// call the media source implementation of the function
		NDIMediaSource->Initialize();

		// the underlying functionality is always return 'true'
		return true;
	}

	// We have no media source to broadcast
	ErrorMessage = TEXT("No Media Source present to broadcast");

	// looks like we don't have a media source to broadcast
	return false;
}

/**
	Changes the name of the sender object as seen on the network for remote connections

	@param InSourceName The new name of the source to be identified as on the network
*/
void UNDIBroadcastComponent::ChangeSourceName(const FString& InSourceName)
{
	// validate the Media Source object
	if (IsValid(NDIMediaSource))
	{
		// call the media source implementation of the function
		NDIMediaSource->ChangeSourceName(InSourceName);
	}
}

/**
	Attempts to change the Broadcast information associated with this media object

	@param InConfiguration The new configuration to broadcast
*/
void UNDIBroadcastComponent::ChangeBroadcastConfiguration(const FNDIBroadcastConfiguration& InConfiguration)
{
	// validate the Media Source object
	if (IsValid(NDIMediaSource))
	{
		// call the media source implementation of the function
		NDIMediaSource->ChangeBroadcastConfiguration(InConfiguration);
	}
}

/**
	Attempts to change the RenderTarget used in sending video frames over NDI

	@param BroadcastTexture The texture to use as video, while broadcasting over NDI
*/
void UNDIBroadcastComponent::ChangeBroadcastTexture(UTextureRenderTarget2D* BroadcastTexture)
{
	// validate the Media Source object
	if (IsValid(NDIMediaSource))
	{
		// call the media source implementation of the function
		NDIMediaSource->ChangeVideoTexture(BroadcastTexture);
	}
}

/**
	Determines the current tally information. If you specify a timeout then it will wait until it has
	changed, otherwise it will simply poll it and return the current tally immediately

	@param IsOnPreview - A state indicating whether this source in on preview of a receiver
	@param IsOnProgram - A state indicating whether this source is on program of a receiver
*/
void UNDIBroadcastComponent::GetTallyInformation(bool& IsOnPreview, bool& IsOnProgram)
{
	// Initialize the properties
	IsOnPreview = false;
	IsOnProgram = false;

	// validate the Media Source object
	if (IsValid(NDIMediaSource))
	{
		// call the media source implementation of the function
		NDIMediaSource->GetTallyInformation(IsOnPreview, IsOnProgram, 0);
	}
}

/**
	Gets the current number of receivers connected to this source. This can be used to avoid rendering
	when nothing is connected to the video source. which can significantly improve the efficiency if
	you want to make a lot of sources available on the network

	@param Result The total number of connected receivers attached to the broadcast of this object
*/
void UNDIBroadcastComponent::GetNumberOfConnections(int32& Result)
{
	// Initialize the property
	Result = 0;

	// validate the Media Source object
	if (IsValid(NDIMediaSource))
	{
		// call the media source implementation of the function
		NDIMediaSource->GetNumberOfConnections(Result);
	}
}

/**
	Attempts to immediately stop sending frames over NDI to any connected receivers
*/
void UNDIBroadcastComponent::StopBroadcasting()
{
	// validate the Media Source object
	if (IsValid(NDIMediaSource))
	{
		// call the media source implementation of the function
		NDIMediaSource->Shutdown();
	}
}