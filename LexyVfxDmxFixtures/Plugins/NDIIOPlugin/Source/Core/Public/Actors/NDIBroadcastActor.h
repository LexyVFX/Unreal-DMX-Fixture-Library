/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#pragma once

#include <CoreMinimal.h>
#include <Components/NDIViewportCaptureComponent.h>

#include "NDIBroadcastActor.generated.h"

/** 
	A quick and easy way to capture the from the perspective of a camera that starts broadcasting the viewport
	immediate upon 'BeginPlay'
*/
UCLASS(BlueprintType, Blueprintable, Category = "NDI IO", META = (DisplayName = "NDI Broadcast Actor"))
class NDIIO_API ANDIBroadcastActor : public AActor
{
	GENERATED_UCLASS_BODY()

	private:
		/** 
			The NDI Media Sender representing the configuration of the network source to send audio, video, and metadata
		*/
		UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "NDI IO", META = (DisplayName = "NDI Media Source", AllowPrivateAccess = true))
		UNDIMediaSender* NDIMediaSource = nullptr;

		/** 
			A component used to capture an additional viewport for broadcasting over NDI
		*/
		UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "NDI IO", META = (DisplayName = "Viewport Capture Component", AllowPrivateAccess = true))
		UNDIViewportCaptureComponent* ViewportCaptureComponent = nullptr;

	public:
		virtual void BeginPlay() override;
};