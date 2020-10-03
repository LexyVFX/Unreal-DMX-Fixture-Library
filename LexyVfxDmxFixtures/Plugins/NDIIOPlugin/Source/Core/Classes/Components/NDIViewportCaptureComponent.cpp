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
#include <Misc/CoreDelegates.h>

UNDIViewportCaptureComponent::UNDIViewportCaptureComponent(const FObjectInitializer& ObjectInitializer)
	: EngineShowFlags(ESFIM_Game), ViewState() {

	this->ViewState.Allocate();

	this->PrimaryComponentTick.bAllowTickOnDedicatedServer = false;
	this->PrimaryComponentTick.bCanEverTick = true;
	this->PrimaryComponentTick.bHighPriority = true;
	this->PrimaryComponentTick.bRunOnAnyThread = false;
	this->PrimaryComponentTick.bStartWithTickEnabled = true;
	this->PrimaryComponentTick.bTickEvenWhenPaused = true;

	this->bWantsInitializeComponent = true;

	this->PostProcessSettings.ReflectionsType = EReflectionsType::ScreenSpace;
	this->PostProcessSettings.ScreenSpaceReflectionIntensity = 90.0f;
	this->PostProcessSettings.ScreenSpaceReflectionMaxRoughness = 0.85f;
	this->PostProcessSettings.ScreenSpaceReflectionQuality = 100.0f;	

	this->PostProcessSettings.bOverride_ReflectionsType = true;
	this->PostProcessSettings.bOverride_ScreenSpaceReflectionIntensity = true;
	this->PostProcessSettings.bOverride_ScreenSpaceReflectionMaxRoughness = true;
	this->PostProcessSettings.bOverride_ScreenSpaceReflectionQuality = true;

	this->PostProcessSettings.MotionBlurAmount = 0.0f;
	this->PostProcessSettings.MotionBlurPerObjectSize = 0.35;
	this->PostProcessSettings.MotionBlurTargetFPS = 60.0f;
	this->PostProcessSettings.MotionBlurMax = 5.0f;

	this->PostProcessSettings.bOverride_MotionBlurAmount = true;
	this->PostProcessSettings.bOverride_MotionBlurMax = true;
	this->PostProcessSettings.bOverride_MotionBlurPerObjectSize = true;
	this->PostProcessSettings.bOverride_MotionBlurTargetFPS = true;

	this->PostProcessSettings.ScreenPercentage = 100.0f;
	this->PostProcessSettings.bOverride_ScreenPercentage = true;

	this->ViewportWidget = SNew(SViewport).
		RenderDirectlyToWindow(false).
		IsEnabled(true).
		EnableGammaCorrection(true).
		EnableBlending(true).
		EnableStereoRendering(false).
		ForceVolatile(true).
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

		// validate the Media Source object
		if (IsValid(NDIMediaSource))
		{
			// define default capture values
			const auto& capture_size = !bOverrideBroadcastSettings ? NDIMediaSource->GetFrameSize() : CaptureSize;
			const auto& capture_rate = !bOverrideBroadcastSettings ? NDIMediaSource->GetFrameRate() : CaptureRate;

			// change the capture sizes as necessary
			ChangeCaptureSettings(capture_size, capture_rate);

			// ensure we are subscribed to the broadcast configuration changed event
			this->NDIMediaSource->OnBroadcastConfigurationChanged.RemoveAll(this);
			this->NDIMediaSource->OnBroadcastConfigurationChanged.AddDynamic(this, &UNDIViewportCaptureComponent::OnBroadcastConfigurationChanged);
		}
	}

	// did we pass validation
	return InMediaSource != nullptr && InMediaSource == NDIMediaSource;
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
	float capture_rate_max = 1 / 1000.0f;
	float capture_rate = CaptureRate.Denominator / (float)CaptureRate.Numerator;

	// set the primary tick interval to the sensible capture rate
	this->PrimaryComponentTick.TickInterval = capture_rate >= capture_rate_max ? capture_rate : -1.0f;

	// Now comes the magic ...
	FTexture2DRHIRef ReferenceTexture = RHICreateTexture2D(
		CaptureSize.X, CaptureSize.Y,
		EPixelFormat::PF_B8G8R8A8, 1, 1,
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

void UNDIViewportCaptureComponent::InitializeComponent()
{
	Super::InitializeComponent();

	// validate the Media Source object
	if (IsValid(NDIMediaSource))
	{
		// define default capture values
		const auto& capture_size = !bOverrideBroadcastSettings ? NDIMediaSource->GetFrameSize() : CaptureSize;
		const auto& capture_rate = !bOverrideBroadcastSettings ? NDIMediaSource->GetFrameRate() : CaptureRate;

		// change the capture sizes as necessary
		ChangeCaptureSettings(capture_size, capture_rate);

		// ensure we are subscribed to the broadcast configuration changed event
		this->NDIMediaSource->OnBroadcastConfigurationChanged.RemoveAll(this);
		this->NDIMediaSource->OnBroadcastConfigurationChanged.AddDynamic(this, &UNDIViewportCaptureComponent::OnBroadcastConfigurationChanged);		
	}}

void UNDIViewportCaptureComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// define some basic parameters dealing with time
	float TimeSeconds = 0.0f, RealTimeSeconds = 0.0f, DeltaTimeSeconds = 0.0f;

	// Only do stuff if we have a valid world pointer
	if (UWorld* WorldContext = UActorComponent::GetWorld())
	{
		// set the time parameters to sometime sensible
		TimeSeconds = WorldContext->GetTimeSeconds();
		RealTimeSeconds = WorldContext->GetRealTimeSeconds();
		DeltaTimeSeconds = WorldContext->GetDeltaSeconds();

		// validate that we are able to perform some broadcasting
		if (IsValid(NDIMediaSource))
		{
			// ensure we have some thread-safety
			FScopeLock Lock(&UpdateRenderContext);

			// Alright we have something that we can use to capture from
			if (ViewFamily != nullptr)
			{
				// Get the number of currently connected clients
				int32 number_of_connections = 0;
				NDIMediaSource->GetNumberOfConnections(number_of_connections);
				NDIMediaSource->PerformsRGBToLinearConversion(false);

				// We should only capture if we have someone connected to us
				if (number_of_connections > 0 && View != nullptr)
				{
					// update the view info with the current camera view (of this component)
					this->GetCameraView(DeltaTimeSeconds, this->ViewInfo);

					// Update the projection matrix
					View->UpdateProjectionMatrix(ViewInfo.CalculateProjectionMatrix());					

					// Update the view matrix
					View->ViewLocation = ViewInfo.Location;
					View->ViewRotation = ViewInfo.Rotation;

					// Do the expensive task of updating the view matrix
					View->UpdateViewMatrix();

					// Start performing post processing
					View->StartFinalPostprocessSettings(ViewInfo.Location);

					// Override the Post Process Setting given the view info
					View->OverridePostProcessSettings(ViewInfo.PostProcessSettings, ViewInfo.PostProcessBlendWeight);

					// Finalize the view settings
					View->EndFinalPostprocessSettings(ViewInitOptions);

					// Do we have something to work with?
					if (UTextureRenderTarget2D* RenderTarget = NDIMediaSource->GetRenderTarget())
					{
						// Create the canvas for rendering the view family
						FCanvas Canvas(this, nullptr, RealTimeSeconds, DeltaTimeSeconds, TimeSeconds, GMaxRHIFeatureLevel);

						// Ensure we empty the extensions, as they get populated within the call to 'BeginRenderingViewFamily'
						ViewFamily->ViewExtensions.Empty();

						// Update the ViewFamily time
						ViewFamily->CurrentRealTime = RealTimeSeconds;
						ViewFamily->CurrentWorldTime = TimeSeconds;
						ViewFamily->DeltaWorldTime = DeltaTimeSeconds;

						// Start Rendering the ViewFamily
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
									RHIUpdateTextureReference(
										MediaTexture->TextureReference.TextureReferenceRHI,
										MediaTexture->Resource->TextureRHI
									);
								}
							});
						}
					}
				}
			}

			// Ensure that the ViewFamily is constructed and matches the capture settings
			else EnsureViewInformation();
		}
	}
}

void UNDIViewportCaptureComponent::CloseRequested(FViewport* Viewport)
{
	if (this->ViewFamily != nullptr)
	{
		delete ViewFamily;
		this->ViewFamily = nullptr;
	}
}

void UNDIViewportCaptureComponent::EnsureViewInformation(bool ForceOverride)
{
	if (ViewFamily == nullptr || ForceOverride)
	{
		// ensure we have some thread-safety
		FScopeLock Lock(&UpdateRenderContext);

		// do some generic memory management
		if (ViewFamily != nullptr)
		{
			delete ViewFamily;
			this->ViewFamily = nullptr;
		}

		// Ensure we have the features we require for nice clean scene capture
		EngineShowFlags.SetOnScreenDebug(false);
		EngineShowFlags.SetSeparateTranslucency(true);
		EngineShowFlags.SetScreenPercentage(true);
		EngineShowFlags.SetTemporalAA(true);

		EngineShowFlags.SetScreenSpaceAO(true);
		EngineShowFlags.SetScreenSpaceReflections(true);

		// Construct the ViewFamily we need to render
		ViewFamily = new FSceneViewFamilyContext(
			FSceneViewFamily::ConstructionValues(this, GetScene(), EngineShowFlags)
			.SetWorldTimes(0.0f, 0.0f, 0.0f)
			.SetRealtimeUpdate(true)
		);

		// Update the ViewFamily with the properties we want to see in the capture
		ViewFamily->ViewMode = VMI_Lit;
		ViewFamily->EngineShowFlags = EngineShowFlags;
		ViewFamily->EngineShowFlags.HitProxies = false;
		ViewFamily->EngineShowFlags.ScreenSpaceReflections = true;
		ViewFamily->EngineShowFlags.ReflectionEnvironment = true;
		ViewFamily->SceneCaptureCompositeMode = SCCM_Additive;
		ViewFamily->Scene = GetScene();
		ViewFamily->bIsHDR = true;
		ViewFamily->bDeferClear = true;
		ViewFamily->bRealtimeUpdate = true;
		ViewFamily->bResolveScene = true;
		ViewFamily->SceneCaptureSource = SCS_FinalColorLDR;

		// Ensure that the viewport is capturing the way we expect it to
		ViewInitOptions.BackgroundColor = FLinearColor(0, 0, 0, 0);
		ViewInitOptions.ViewFamily = ViewFamily;
		ViewInitOptions.SceneViewStateInterface = ViewState.GetReference();
		ViewInitOptions.StereoPass = eSSP_FULL;
		ViewInitOptions.bUseFieldOfViewForLOD = ViewInfo.bUseFieldOfViewForLOD;
		ViewInitOptions.FOV = ViewInfo.FOV;
		ViewInitOptions.OverrideFarClippingPlaneDistance = 100000.0f;
		ViewInitOptions.CursorPos = FIntPoint(-1, -1);
		ViewInitOptions.ViewOrigin = ViewInfo.Location;
		ViewInitOptions.SetViewRectangle(FIntRect(0, 0, CaptureSize.X, CaptureSize.Y));

		// Initialize the FOV to what we expect
		ViewInfo.FOV = this->FieldOfView;

		// Generate a default view rotation
		ViewInitOptions.ViewRotationMatrix = FInverseRotationMatrix(ViewInfo.Rotation) * FMatrix(
			FPlane(0, 0, 1, 0),
			FPlane(1, 0, 0, 0),
			FPlane(0, 1, 0, 0),
			FPlane(0, 0, 0, 1)
		);

		// 
		FMinimalViewInfo::CalculateProjectionMatrixGivenView(
			ViewInfo,
			EAspectRatioAxisConstraint::AspectRatio_MajorAxisFOV,
			SceneViewport.Get(),
			ViewInitOptions
		);

		// This is required by the 'BeginRenderingViewFamily' function call
		if (ViewFamily->GetScreenPercentageInterface() == nullptr)
			ViewFamily->SetScreenPercentageInterface(new FLegacyScreenPercentageDriver(*ViewFamily, 1.0f, true));

		// We need cannot create a view ourselves, so we have to call into the engine to do this for us, but
		// the requirement is to be on the render thread....
		ENQUEUE_RENDER_COMMAND(FNDIViewportCaptureServiceUpdateViewFamily)(
			[&](FRHICommandListImmediate& RHICmdList) {

			// ensure we have some thread-safety
			FScopeLock Lock(&UpdateRenderContext);

			GetRendererModule().CreateAndInitSingleView(RHICmdList, ViewFamily, &ViewInitOptions);

			View = const_cast<FSceneView*>(ViewFamily->Views[0]);
		});
	}
}

void UNDIViewportCaptureComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (IsValid(NDIMediaSource))
	{
		// unsubscribe from the broadcast configuration change notification
		NDIMediaSource->OnBroadcastConfigurationChanged.RemoveAll(this);
	}

	FCoreDelegates::OnBeginFrame.RemoveAll(this);
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

		// Ensure that the ViewFamily is constructed and matches the capture settings
		EnsureViewInformation(true);
	}
}

//////////////////////////////////////////////////////////////////////////

FIntPoint UNDIViewportCaptureComponent::GetSizeXY() const { return this->CaptureSize; }

float UNDIViewportCaptureComponent::GetDisplayGamma() const { return 2.2f; }

const FTexture2DRHIRef& UNDIViewportCaptureComponent::GetRenderTargetTexture() const { return (FTexture2DRHIRef&)RenderableTexture; }
