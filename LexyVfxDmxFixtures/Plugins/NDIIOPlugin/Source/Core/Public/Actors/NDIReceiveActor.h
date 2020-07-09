/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#pragma once

#include <CoreMinimal.h>
#include <Components/AudioComponent.h>
#include <Components/StaticMeshComponent.h>
#include <Components/NDIReceiverComponent.h>

#include "NDIReceiveActor.generated.h"


UCLASS(HideCategories = (Activation, Rendering, AssetUserData, Material, Attachment, Actor, Input, Cooking, LOD), Category = "NDI IO", META =(DisplayName = "NDI Receive Actor"))
class NDIIO_API ANDIReceiveActor : public AActor
{
	GENERATED_UCLASS_BODY()

	private:
		/** The desired height of the frame in cm, represented in the virtual scene */
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Interp, BlueprintSetter = "SetFrameHeight", Category = "Properties", META = (DisplayName = "Frame Height", AllowPrivateAccess = true))
		float FrameHeight = 100.0f;

		/** The desired width of the frame in cm, represented in the virtual scene */
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Interp, BlueprintSetter = "SetFrameWidth", Category = "Properties", META = (DisplayName = "Frame Width", AllowPrivateAccess = true))
		float FrameWidth = 178.887;

		/** The component used to playback audio received from the Media Sender object */
		UPROPERTY(VisibleAnywhere, Category = "Components", META = (DisplayName = "Audio Component", AllowPrivateAccess = true))
		UAudioComponent* AudioComponent = nullptr;

		/** The component used to display the video received from the Media Sender object */
		UPROPERTY(VisibleAnywhere, Category = "Components", META = (DisplayName = "Video Mesh Component", AllowPrivateAccess = true))
		UStaticMeshComponent* VideoMeshComponent = nullptr;

		/** The Receiver object used to get Audio, Video, and Metadata from on the network */
		UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category = "Components", META = (DisplayName = "NDI Media Source", AllowPrivateAccess = true))
		UNDIMediaReceiver* NDIMediaSource = nullptr;

	public:
		virtual void BeginPlay() override;

		/** 
			Attempts to set the desired frame size in cm, represented in the virtual scene
		*/
		void SetFrameSize(FVector2D InFrameSize);

		/** 
			Returns the current frame size of the 'VideoMeshComponent' for this object
		*/
		const FVector2D GetFrameSize() const;

	private:
		UFUNCTION(BlueprintSetter)
		void SetFrameHeight(const float& InFrameHeight);

		UFUNCTION(BlueprintSetter)
		void SetFrameWidth(const float& InFrameWidth);
		
		#if WITH_EDITORONLY_DATA

		virtual void PreEditChange(FProperty* InProperty) override;
		virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

		#endif
};