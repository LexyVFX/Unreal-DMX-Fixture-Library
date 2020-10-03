/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#pragma once

#include <CoreMinimal.h>

#include <CineCameraComponent.h>
#include <Engine/TextureRenderTarget2D.h>
#include <Objects/Media/NDIMediaSender.h>
#include <Misc/FrameRate.h>
#include <Framework/Application/SlateApplication.h>
#include <SceneManagement.h>
#include <Slate/SceneViewport.h>
#include <Widgets/SViewport.h>
#include <Widgets/SWindow.h>

#include "NDIViewportCaptureComponent.generated.h"

/** 
	A component used to capture an additional viewport for broadcasting over NDI
*/
UCLASS(BlueprintType, Blueprintable, Category = "NDI IO", META = (DisplayName = "NDI Viewport Capture Component", BlueprintSpawnableComponent))
class NDIIO_API UNDIViewportCaptureComponent : public UCineCameraComponent, public FCommonViewportClient, public FRenderTarget
{
	GENERATED_UCLASS_BODY()

	private:
		/** 
			If true, will allow you to override the capture settings by ignoring the default Broadcast Settings
			in the NDI Media Sender, Potentially Requiring a texture rescale of the capture frame when broadcasting
			over NDI.
		*/
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Capture Settings", META = (AllowPrivateAccess = true))
		bool bOverrideBroadcastSettings = false;
		
		/** 
			Describes the Height and Width of the viewport frame to capture.			
		*/
		UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Capture Settings", META = (DisplayName = "Capture Size", AllowPrivateAccess = true, EditCondition = "bOverrideBroadcastSettings"))
		FIntPoint CaptureSize = FIntPoint(1280, 720);

		/** 
			Represents the desired number of frames (per second) to capture the viewport 
		*/
		UPROPERTY(BlueprintReadwrite, EditAnywhere, Category = "Capture Settings", META = (DisplayName = "Capture Rate", AllowPrivateAccess = true, EditCondition = "bOverrideBroadcastSettings"))
		FFrameRate CaptureRate = FFrameRate(60, 1);
			
		/** 
			The NDI Media Sender representing the configuration of the network source to send audio, video, and metadata 
		*/
		UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Properties", META = (DisplayName = "NDI Media Source", AllowPrivateAccess = true))
		UNDIMediaSender* NDIMediaSource = nullptr;

	public:
		/** 
			Initialize this component with the media source required for sending NDI audio, video, and metadata.
			Returns false, if the MediaSource is already been set. This is usually the case when this component is
			initialized in Blueprints.
		*/
		bool Initialize(UNDIMediaSender* InMediaSource = nullptr);

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
			Change the capture settings of the viewport capture and overrides the NDI Media Sender settings

			@param InCaptureSize The Capture size of the frame to capture of the viewport
			@param InCaptureRate A framerate at which to capture frames of the viewport
		*/
		UFUNCTION(BlueprintCallable, Category = "NDI IO", META = (DisplayName = "Change Capture Settings"))
		void ChangeCaptureSettings(FIntPoint InCaptureSize, FFrameRate InCaptureRate);

		/**
			Determines the current tally information. If you specify a timeout then it will wait until it has
			changed, otherwise it will simply poll it and return the current tally immediately

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

	protected:
		virtual void InitializeComponent() override;
		virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
		virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

		virtual FIntPoint GetSizeXY() const override;

		virtual float GetDisplayGamma() const override;
		virtual const FTexture2DRHIRef& GetRenderTargetTexture() const override;

		virtual void CloseRequested(FViewport* Viewport) override;

	private:
		void EnsureViewInformation(bool ForceOverride = true);

		UFUNCTION()
		void OnBroadcastConfigurationChanged(UNDIMediaSender* Sender);

	private:
		FMinimalViewInfo ViewInfo;
		FEngineShowFlags EngineShowFlags;
		FSceneViewStateReference ViewState;
		FSceneViewInitOptions ViewInitOptions;

		FSceneView* View = nullptr;
		FSceneViewFamilyContext* ViewFamily = nullptr;

		FCriticalSection UpdateRenderContext;

		TSharedPtr<SViewport> ViewportWidget = nullptr;
		TSharedPtr<FSceneViewport> SceneViewport = nullptr;
		FTexture2DRHIRef RenderableTexture = nullptr;
};