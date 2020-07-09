/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#include <Components/NDIViewportCaptureComponent.h>
#include <Rendering/RenderingCommon.h>
#include <SceneView.h>
#include <SceneViewExtension.h>
#include <CanvasTypes.h>
#include <EngineModule.h>
#include <LegacyScreenPercentageDriver.h>
#include <RenderResource.h>
#include <UnrealClient.h>
#include <Engine/Engine.h>
#include <EngineUtils.h>
#include <Runtime/Renderer/Private/ScenePrivate.h>

UNDIViewportCaptureComponent::UNDIViewportCaptureComponent(const FObjectInitializer& ObjectInitializer)
	: EngineShowFlags(ESFIM_Game), ViewState() {

	this->ViewState.Allocate();	

	this->PrimaryComponentTick.bAllowTickOnDedicatedServer = false;
	this->PrimaryComponentTick.bCanEverTick = true;
	this->PrimaryComponentTick.bHighPriority = true;
	this->PrimaryComponentTick.bRunOnAnyThread = false;
	this->PrimaryComponentTick.bStartWithTickEnabled = true;
	this->PrimaryComponentTick.bTickEvenWhenPaused = true;	

	this->PostProcessSettings.AutoExposureBias = 0.5f;
	this->PostProcessSettings.bOverride_AutoExposureBias = true;	

	this->ViewportWidget = SNew(SViewport).RenderDirectlyToWindow(false).
		IsEnabled(true).
		EnableGammaCorrection(true).
		EnableBlending(false).
		IgnoreTextureAlpha(true);

	this->SceneViewport = MakeShareable(new FSceneViewport(this, ViewportWidget));
	this->ViewportWidget->SetViewportInterface(this->SceneViewport.ToSharedRef());
}

/**
	Initialize this component with the media source required for sending NDI audio, video, and metadata.
	Returns false, if the MediaSource is already been set. This is usually the case when this component is
	initialized in Blueprints.
*/
bool UNDIViewportCaptureComponent::Initialize(UNDIMediaSender* InMediaSource)
{
	// is the media source already set?
	if (this->NDIMediaSource == nullptr && InMediaSource != nullptr)
	{
		// we passed validation, so set the media source
		this->NDIMediaSource = InMediaSource;		
	}

	// did we pass validation
	return InMediaSource != nullptr && InMediaSource == NDIMediaSource;
}

/**
	Attempts to start broadcasting audio, video, and metadata via the 'NDIMediaSource' associated with this object

	@param ErrorMessage The error message received when the media source is unable to start broadcasting
	@result Indicates whether this object successfully started broadcasting
*/
bool UNDIViewportCaptureComponent::StartBroadcasting(FString& ErrorMessage)
{
	// validate the Media Source object
	if (IsValid(NDIMediaSource))
	{
		// define default capture values
		const auto& capture_size = !bOverrideBroadcastSettings ? NDIMediaSource->GetFrameSize() : CaptureSize;
		const auto& capture_rate = !bOverrideBroadcastSettings ? NDIMediaSource->GetFrameRate() : CaptureRate;

		// change the capture sizes as necessary
		ChangeCaptureSettings(capture_size, capture_rate);

		// call the media source implementation of the function
		NDIMediaSource->Initialize();

		// ensure we are subscribed to the broadcast configuration changed event
		this->NDIMediaSource->OnBroadcastConfigurationChanged.AddDynamic(this, &UNDIViewportCaptureComponent::OnBroadcastConfigurationChanged);

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
void UNDIViewportCaptureComponent::ChangeSourceName(const FString& InSourceName)
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
void UNDIViewportCaptureComponent::ChangeBroadcastConfiguration(const FNDIBroadcastConfiguration& InConfiguration)
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
void UNDIViewportCaptureComponent::ChangeBroadcastTexture(UTextureRenderTarget2D* BroadcastTexture)
{
	// validate the Media Source object
	if (IsValid(NDIMediaSource))
	{
		// call the media source implementation of the function
		NDIMediaSource->ChangeVideoTexture(BroadcastTexture);
	}
}

/**
	Change the capture settings of the viewport capture

	@param InCaptureSize The Capture size of the frame to capture of the viewport
	@param InCaptureRate A framerate at which to capture frames of the viewport
*/
void UNDIViewportCaptureComponent::ChangeCaptureSettings(FIntPoint InCaptureSize, FFrameRate InCaptureRate)
{
	// initialize the resource create information structure
	FRHIResourceCreateInfo CreateInfo = { FClearValueBinding(FLinearColor(0.0f, 0.0f, 0.0f)) };

	// clamp our viewport capture size
	int32 capture_width = FMath::Max(InCaptureSize.X, 64);
	int32 capture_height = FMath::Max(InCaptureSize.Y, 64);

	// set the capture size
	this->CaptureSize = FIntPoint(capture_width, capture_height);

	// set the capture rate
	this->CaptureRate = InCaptureRate;

	// clamp the maximum capture rate to something reasonable
	float capture_rate_max = 1000.0f / 1.0f;
	float capture_rate = CaptureRate.Denominator / (float)CaptureRate.Numerator;
	
	// set the primary tick interval to the sensible capture rate
	this->PrimaryComponentTick.TickInterval = capture_rate <= capture_rate_max ? capture_rate : -1.0f;

	// Now comes the magic ...
	FTexture2DRHIRef ReferenceTexture = RHICreateTexture2D(
		CaptureSize.X, CaptureSize.Y,
		PF_B8G8R8A8, 1, 1,
		TexCreate_CPUReadback,
		CreateInfo
	);

	// create the targetable shader resource so that we can 'capture' and upload to the media sender object
	RHICreateTargetableShaderResource2D(
		CaptureSize.X,
		CaptureSize.Y,
		EPixelFormat::PF_B8G8R8A8,
		1,
		TexCreate_Dynamic,
		TexCreate_RenderTargetable,
		false,
		CreateInfo,
		RenderableTexture,
		ReferenceTexture
	);	
}

/**
	Determines the current tally information. If you specify a timeout then it will wait until it has
	changed, otherwise it will simply poll it and return the current tally immediately

	@param IsOnPreview - A state indicating whether this source in on preview of a receiver
	@param IsOnProgram - A state indicating whether this source is on program of a receiver
*/
void UNDIViewportCaptureComponent::GetTallyInformation(bool& IsOnPreview, bool& IsOnProgram)
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
void UNDIViewportCaptureComponent::GetNumberOfConnections(int32& Result)
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
void UNDIViewportCaptureComponent::StopBroadcasting()
{
	// validate the Media Source object
	if (IsValid(NDIMediaSource))
	{
		// call the media source implementation of the function
		NDIMediaSource->Shutdown();

		// reset the view family
		// this->ViewFamily = nullptr;

		// unsubscribe from the broadcast configuration change notification
		NDIMediaSource->OnBroadcastConfigurationChanged.RemoveAll(this);
	}
}

void UNDIViewportCaptureComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// define some basic parameters dealing with time
	float TimeSeconds = 0.0f, RealTimeSeconds = 0.0f, DeltaTimeSeconds = 0.0f;

	// Only do stuff if we have a valid world pointer
	if (UWorld* WorldContext = UActorComponent::GetWorld())
	{
		// ensure we have some thread-safety
		FScopeLock Lock(&UpdateRenderContext);

		// set the time parameters to sometime sensible
		TimeSeconds = WorldContext->GetTimeSeconds();
		RealTimeSeconds = WorldContext->GetRealTimeSeconds();
		DeltaTimeSeconds = WorldContext->GetDeltaSeconds();

		// validate that we are able to perform some broadcasting
		if (IsValid(NDIMediaSource))
		{
			// Alright we have something that we can use to capture from
			if (ViewFamily != nullptr)
			{
				// Get the number of currently connected clients
				int32 number_of_connections = 0;
				NDIMediaSource->GetNumberOfConnections(number_of_connections);
				NDIMediaSource->PerformsRGBToLinearConversion(false);

				// We should only capture if we have someone connected to us
				if (number_of_connections > 0)
				{
					// update the view info with the current camera view (of this component)
					this->GetCameraView(DeltaTimeSeconds, this->ViewInfo);

					// update the view's location and rotation
					View->ViewLocation = ViewInfo.Location;
					View->ViewRotation = ViewInfo.Rotation;					
					View->UpdateViewMatrix();

					// change the init options so that we are rendering from the correct viewpoint
					ViewInitOptions.FOV = ViewInfo.FOV;
					ViewInitOptions.ViewOrigin = ViewInfo.Location;
					ViewInitOptions.bUseFieldOfViewForLOD = ViewInfo.bUseFieldOfViewForLOD;
					ViewInitOptions.SetViewRectangle(FIntRect(0, 0, CaptureSize.X, CaptureSize.Y));
					ViewInitOptions.ViewRotationMatrix = FInverseRotationMatrix(ViewInfo.Rotation) * FMatrix(
						FPlane(0, 0, 1, 0),
						FPlane(1, 0, 0, 0),
						FPlane(0, 1, 0, 0),
						FPlane(0, 0, 0, 1)
					);					

					// re-calculate the projection matrix
					FMinimalViewInfo::CalculateProjectionMatrixGivenView(ViewInfo, EAspectRatioAxisConstraint::AspectRatio_MajorAxisFOV, SceneViewport.Get(), ViewInitOptions);
					
					View->StartFinalPostprocessSettings(ViewInfo.Location);
					View->OverridePostProcessSettings(ViewInfo.PostProcessSettings, ViewInfo.PostProcessBlendWeight);
					View->EndFinalPostprocessSettings(ViewInitOptions);

					if (UTextureRenderTarget2D* RenderTarget = NDIMediaSource->GetRenderTarget())
					{
						FCanvas Canvas(this, nullptr, RealTimeSeconds, DeltaTimeSeconds, TimeSeconds, GMaxRHIFeatureLevel);
						Canvas.SetAllowedModes(0);

						ViewFamily->ViewExtensions.Empty();
						GetRendererModule().BeginRenderingViewFamily(&Canvas, ViewFamily);						

						if (RenderTarget->Resource->TextureRHI != RenderableTexture || 
							RenderTarget->Resource->TextureRHI->GetSizeXYZ() != RenderableTexture->GetSizeXYZ()) {

							// change the texture rhi to the renderable texture
							RenderTarget->Resource->TextureRHI = (FTexture2DRHIRef&)RenderableTexture;

							ENQUEUE_RENDER_COMMAND(FNDIViewportCaptureServiceUpdateTextureBackbuffer)(
								[&](FRHICommandListImmediate& RHICmdList) {

								// validate the media texture
								if (UTextureRenderTarget2D* MediaTexture = NDIMediaSource->GetRenderTarget())
								{
									// ensure that the texture reference is the same as the renderable texture
									RHIUpdateTextureReference(MediaTexture->TextureReference.TextureReferenceRHI, MediaTexture->Resource->TextureRHI);
								}
							});
						}
					}
				}
			}

			// this must be the first time we have 'updated' so lets create the view family and capture the scene next frame
			else if (ViewFamily == nullptr)
			{
				// Ensure we do not have the advanced features turned on
				EngineShowFlags.DisableAdvancedFeatures();
				EngineShowFlags.SetMotionBlur(false);
				EngineShowFlags.SetTemporalAA(true);

				ViewFamily = new FSceneViewFamilyContext(
					FSceneViewFamily::ConstructionValues(this, GetScene(), EngineShowFlags)
					.SetWorldTimes(0.0f, 0.0f, 0.0f)
					.SetRealtimeUpdate(true)
				);

				ViewFamily->ViewMode = VMI_Lit;
				ViewFamily->EngineShowFlags = EngineShowFlags;				
				ViewFamily->Scene = GetScene();

				ViewInitOptions.BackgroundColor = FLinearColor(0, 0, 0, 0);
				ViewInitOptions.ViewFamily = ViewFamily;
				ViewInitOptions.SceneViewStateInterface = ViewState.GetReference();
				ViewInitOptions.StereoPass = eSSP_FULL;				
				ViewInitOptions.bUseFieldOfViewForLOD = ViewInfo.bUseFieldOfViewForLOD;
				ViewInitOptions.FOV = ViewInfo.FOV;
				ViewInitOptions.OverrideFarClippingPlaneDistance = 0.0f;
				ViewInitOptions.CursorPos = FIntPoint(-1, -1);
				ViewInitOptions.ViewOrigin = ViewInfo.Location;
				ViewInitOptions.SetViewRectangle(FIntRect(0, 0, CaptureSize.X, CaptureSize.Y));

				this->ViewInfo.FOV = this->FieldOfView;

				ViewInitOptions.ViewRotationMatrix = FInverseRotationMatrix(ViewInfo.Rotation) * FMatrix(
					FPlane(0, 0, 1, 0),
					FPlane(1, 0, 0, 0),
					FPlane(0, 1, 0, 0),
					FPlane(0, 0, 0, 1)
				);

				FMinimalViewInfo::CalculateProjectionMatrixGivenView(ViewInfo, EAspectRatioAxisConstraint::AspectRatio_MajorAxisFOV, SceneViewport.Get(), ViewInitOptions);

				if (ViewFamily->GetScreenPercentageInterface() == nullptr)
					ViewFamily->SetScreenPercentageInterface(new FLegacyScreenPercentageDriver(*ViewFamily, 1.0f, false));

				ENQUEUE_RENDER_COMMAND(FNDIViewportCaptureServiceUpdateViewFamily)(
					[&](FRHICommandListImmediate& RHICmdList) {
					
					GetRendererModule().CreateAndInitSingleView(RHICmdList, ViewFamily, &ViewInitOptions);

					this->View = const_cast<FSceneView*>(ViewFamily->Views[0]);

					if (ViewFamily->EngineShowFlags.Wireframe)
					{
						// Wireframe color is emissive-only, and mesh-modifying materials do not use material substitution, hence...
						View->DiffuseOverrideParameter = FVector4(0.f, 0.f, 0.f, 0.f);
						View->SpecularOverrideParameter = FVector4(0.f, 0.f, 0.f, 0.f);
					}
					else if (ViewFamily->EngineShowFlags.OverrideDiffuseAndSpecular)
					{
						View->DiffuseOverrideParameter = FVector4(0.78f, 0.78f, 0.78f, 0.0f);
						View->SpecularOverrideParameter = FVector4(.1f, .1f, .1f, 0.0f);
					}
					else if (ViewFamily->EngineShowFlags.ReflectionOverride)
					{
						View->DiffuseOverrideParameter = FVector4(0.f, 0.f, 0.f, 0.f);
						View->SpecularOverrideParameter = FVector4(1, 1, 1, 0.0f);
						View->NormalOverrideParameter = FVector4(0, 0, 1, 0.0f);
						View->RoughnessOverrideParameter = FVector2D(0.0f, 0.0f);
					}

					if (!ViewFamily->EngineShowFlags.Diffuse)
						View->DiffuseOverrideParameter = FVector4(0.f, 0.f, 0.f, 0.f);

					if (!ViewFamily->EngineShowFlags.Specular)
						View->SpecularOverrideParameter = FVector4(0.f, 0.f, 0.f, 0.f);

					View->CurrentBufferVisualizationMode = NAME_None;
				});
			}

		}
	}
}

FIntPoint UNDIViewportCaptureComponent::GetSizeXY() const { return this->CaptureSize; }

float UNDIViewportCaptureComponent::GetDisplayGamma() const { return 2.2f; }

const FTexture2DRHIRef& UNDIViewportCaptureComponent::GetRenderTargetTexture() const
{
	return (FTexture2DRHIRef&)RenderableTexture;
}

void UNDIViewportCaptureComponent::CloseRequested(FViewport* Viewport)
{
	if (this->ViewFamily != nullptr)
	{
		delete ViewFamily;
		this->ViewFamily = nullptr;
	}
}

void UNDIViewportCaptureComponent::OnBroadcastConfigurationChanged(UNDIMediaSender* Sender)
{
	// If we are not overriding the broadcast settings and the sender is valid
	if (!bOverrideBroadcastSettings && IsValid(Sender))
	{
		// ensure we have some thread-safety
		FScopeLock Lock(&UpdateRenderContext);

		// change the capture sizes as necessary
		ChangeCaptureSettings(Sender->GetFrameSize(), Sender->GetFrameRate());

		ENQUEUE_RENDER_COMMAND(FNDIViewportCaptureServiceUpdateViewFamily)(
			[&](FRHICommandListImmediate& RHICmdList) {

			// ensure we have some thread-safety
			FScopeLock Lock(&UpdateRenderContext);

			// Ensure we do not have the advanced features turned on
			EngineShowFlags.DisableAdvancedFeatures();
			EngineShowFlags.SetMotionBlur(false);
			EngineShowFlags.SetTemporalAA(true);

			ViewFamily = new FSceneViewFamilyContext(
				FSceneViewFamily::ConstructionValues(this, GetScene(), EngineShowFlags)
				.SetWorldTimes(0.0f, 0.0f, 0.0f)
				.SetRealtimeUpdate(true)
			);

			ViewFamily->ViewMode = VMI_Lit;
			ViewFamily->EngineShowFlags = EngineShowFlags;
			ViewFamily->Scene = GetScene();

			ViewInitOptions.BackgroundColor = FLinearColor(0, 0, 0, 0);
			ViewInitOptions.ViewFamily = ViewFamily;
			ViewInitOptions.SceneViewStateInterface = ViewState.GetReference();
			ViewInitOptions.StereoPass = eSSP_FULL;
			ViewInitOptions.bUseFieldOfViewForLOD = ViewInfo.bUseFieldOfViewForLOD;
			ViewInitOptions.FOV = ViewInfo.FOV;
			ViewInitOptions.OverrideFarClippingPlaneDistance = 0.0f;
			ViewInitOptions.CursorPos = FIntPoint(-1, -1);
			ViewInitOptions.ViewOrigin = ViewInfo.Location;
			ViewInitOptions.SetViewRectangle(FIntRect(0, 0, CaptureSize.X, CaptureSize.Y));

			this->ViewInfo.FOV = this->FieldOfView;

			ViewInitOptions.ViewRotationMatrix = FInverseRotationMatrix(ViewInfo.Rotation) * FMatrix(
				FPlane(0, 0, 1, 0),
				FPlane(1, 0, 0, 0),
				FPlane(0, 1, 0, 0),
				FPlane(0, 0, 0, 1)
			);

			FMinimalViewInfo::CalculateProjectionMatrixGivenView(ViewInfo, EAspectRatioAxisConstraint::AspectRatio_MajorAxisFOV, SceneViewport.Get(), ViewInitOptions);

			if (ViewFamily->GetScreenPercentageInterface() == nullptr)
				ViewFamily->SetScreenPercentageInterface(new FLegacyScreenPercentageDriver(*ViewFamily, 1.0f, false));

			GetRendererModule().CreateAndInitSingleView(RHICmdList, ViewFamily, &ViewInitOptions);

			this->View = const_cast<FSceneView*>(ViewFamily->Views[0]);

			if (ViewFamily->EngineShowFlags.Wireframe)
			{
				// Wireframe color is emissive-only, and mesh-modifying materials do not use material substitution, hence...
				View->DiffuseOverrideParameter = FVector4(0.f, 0.f, 0.f, 0.f);
				View->SpecularOverrideParameter = FVector4(0.f, 0.f, 0.f, 0.f);
			}
			else if (ViewFamily->EngineShowFlags.OverrideDiffuseAndSpecular)
			{
				View->DiffuseOverrideParameter = FVector4(0.78f, 0.78f, 0.78f, 0.0f);
				View->SpecularOverrideParameter = FVector4(.1f, .1f, .1f, 0.0f);
			}
			else if (ViewFamily->EngineShowFlags.ReflectionOverride)
			{
				View->DiffuseOverrideParameter = FVector4(0.f, 0.f, 0.f, 0.f);
				View->SpecularOverrideParameter = FVector4(1, 1, 1, 0.0f);
				View->NormalOverrideParameter = FVector4(0, 0, 1, 0.0f);
				View->RoughnessOverrideParameter = FVector2D(0.0f, 0.0f);
			}

			if (!ViewFamily->EngineShowFlags.Diffuse)
				View->DiffuseOverrideParameter = FVector4(0.f, 0.f, 0.f, 0.f);

			if (!ViewFamily->EngineShowFlags.Specular)
				View->SpecularOverrideParameter = FVector4(0.f, 0.f, 0.f, 0.f);

			View->CurrentBufferVisualizationMode = NAME_None;
		});
	}
}
