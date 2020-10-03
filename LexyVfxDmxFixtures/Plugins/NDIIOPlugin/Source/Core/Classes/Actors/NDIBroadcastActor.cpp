/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#include <Actors/NDIBroadcastActor.h>

ANDIBroadcastActor::ANDIBroadcastActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {
	
	this->ViewportCaptureComponent = ObjectInitializer.CreateDefaultSubobject<UNDIViewportCaptureComponent>(this, TEXT("ViewportCaptureComponent"));
	this->ViewportCaptureComponent->AttachToComponent(this->RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void ANDIBroadcastActor::BeginPlay()
{
	Super::BeginPlay();

	// validate the viewport capture component
	if (IsValid(this->ViewportCaptureComponent))
	{
		// Initialize the Capture Component with the media source
		ViewportCaptureComponent->Initialize(this->NDIMediaSource);
	}
}