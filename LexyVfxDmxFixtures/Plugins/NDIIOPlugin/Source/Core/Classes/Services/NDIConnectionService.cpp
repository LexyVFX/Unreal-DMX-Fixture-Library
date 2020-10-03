/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#include <Services/NDIConnectionService.h>

#include <Misc/CoreDelegates.h>
#include <NDIIOPluginSettings.h>
#include <Objects/Media/NDIMediaSender.h>
#include <FrameWork/Application/SlateApplication.h>

#if WITH_EDITOR

	#include <Editor.h>

#endif

/** Define Global Accessors */

bool FNDIConnectionService::IsInEditorMode = false;
FNDIConnectionServiceSendEvent FNDIConnectionService::EventOnSendAudioFrame;
FNDIConnectionServiceSendEvent FNDIConnectionService::EventOnSendVideoFrame;

/** ************************ **/

/**
	Constructs a new instance of this object
*/
FNDIConnectionService::FNDIConnectionService()
{

}

// Begin the service
bool FNDIConnectionService::Start()
{
	if (!bIsInitialized)
	{
		bIsInitialized = true;

		// Define some basic properties
		FNDIBroadcastConfiguration Configuration;
		FString BroadcastName = TEXT("Unreal Engine");		
		EObjectFlags Flags = RF_Public| RF_Standalone | RF_Transient | RF_MarkAsNative;

		// Load the plugin settings for broadcasting the active viewport
		if (auto* CoreSettings = NewObject<UNDIIOPluginSettings>())
		{
			// Define the configuration properties
			Configuration.FrameRate = CoreSettings->BroadcastRate;
			Configuration.FrameSize = FIntPoint(
				FMath::Clamp(CoreSettings->PreferredFrameSize.X, 240, 3840),
				FMath::Clamp(CoreSettings->PreferredFrameSize.Y, 240, 3840)
			);
			
			// Set the broadcast name
			BroadcastName = CoreSettings->ApplicationStreamName;

			// clean-up the settings object
			CoreSettings->ConditionalBeginDestroy();
			CoreSettings = nullptr;
		}

		/** Construct the Active Viewport video texture */
		this->VideoTexture = NewObject<UTextureRenderTarget2D>(
			GetTransientPackage(),
			UTextureRenderTarget2D::StaticClass(),
			TEXT("NDIViewportVideoTexture"),
			Flags
		);

		/** Construct the active viewport sender */
		this->ActiveViewportSender = NewObject<UNDIMediaSender>(
			GetTransientPackage(),
			UNDIMediaSender::StaticClass(),
			TEXT("NDIViewportSender"),
			Flags
		);

		VideoTexture->UpdateResource();
		
		// Update the active viewport sender, with the properties defined in the settings configuration
		this->ActiveViewportSender->ChangeSourceName(BroadcastName);
		this->ActiveViewportSender->ChangeVideoTexture(VideoTexture);
		this->ActiveViewportSender->ChangeBroadcastConfiguration(Configuration);		
		
		// hook into the core for the end of frame handlers
		FCoreDelegates::OnEndFrame.AddRaw(this, &FNDIConnectionService::OnEndFrame);
		FCoreDelegates::OnEndFrameRT.AddRaw(this, &FNDIConnectionService::OnEndRenderFrame);

		#if WITH_EDITOR

		FEditorDelegates::BeginPIE.AddLambda([&](const bool Success) { IsInEditorMode = true; });
		FEditorDelegates::PrePIEEnded.AddLambda([&](const bool Success) { StopBroadcastingActiveViewport(); });

		#endif
	}

	return true;
}

// Stop the service
void FNDIConnectionService::Shutdown()
{
	// Wait for the sync context locks
	FScopeLock AudioLock(&AudioSyncContext);
	FScopeLock RenderLock(&RenderSyncContext);

	// reset the initialization properties	
	bIsInitialized = false;	

	// unbind our handlers for our frame events
	FCoreDelegates::OnEndFrame.RemoveAll(this);
	FCoreDelegates::OnEndFrameRT.RemoveAll(this);

	// Cleanup the broadcasting of the active viewport
	StopBroadcastingActiveViewport();
}

// Handler for when the Non render thread frame has ended
void FNDIConnectionService::OnEndFrame()
{
	FScopeLock Lock(&AudioSyncContext);

	if (bIsInitialized)
	{
		int64 ticks = FDateTime::Now().GetTimeOfDay().GetTicks();

		if (FNDIConnectionService::EventOnSendAudioFrame.IsBound())
		{
			FNDIConnectionService::EventOnSendAudioFrame.Broadcast(ticks);
		}	
	}
}

// Handler for when the render thread frame has ended
void FNDIConnectionService::OnEndRenderFrame()
{
	FScopeLock Lock(&RenderSyncContext);

	if (bIsInitialized)
	{
		int64 ticks = FDateTime::Now().GetTimeOfDay().GetTicks();		

		if (FNDIConnectionService::EventOnSendVideoFrame.IsBound())
		{
			FNDIConnectionService::EventOnSendVideoFrame.Broadcast(ticks);
		}	
	}
}

bool FNDIConnectionService::BeginBroadcastingActiveViewport()
{
	if (!bIsBroadcastingActiveViewport && IsValid(ActiveViewportSender))
	{
		#if UE_EDITOR

		// Load the plugin settings for broadcasting the active viewport
		if (auto* CoreSettings = NewObject<UNDIIOPluginSettings>())
		{
			// Define some basic properties
			FNDIBroadcastConfiguration Configuration;
			FString BroadcastName = TEXT("Unreal Engine");

			// Define the configuration properties
			Configuration.FrameRate = CoreSettings->BroadcastRate;
			Configuration.FrameSize = FIntPoint(
				FMath::Clamp(CoreSettings->PreferredFrameSize.X, 240, 3840),
				FMath::Clamp(CoreSettings->PreferredFrameSize.Y, 240, 3840)
			);

			// Set the broadcast name
			BroadcastName = CoreSettings->ApplicationStreamName;

			// clean-up the settings object
			CoreSettings->ConditionalBeginDestroy();
			CoreSettings = nullptr;

			// Update the active viewport sender, with the properties defined in the settings configuration
			this->ActiveViewportSender->ChangeSourceName(BroadcastName);
			this->ActiveViewportSender->ChangeBroadcastConfiguration(Configuration);
		}

		#endif

		// we don't want to perform the linear conversion for the active viewport, 
		// since it's already had the conversion completed by the engine before passing to the sender
		ActiveViewportSender->PerformsRGBToLinearConversion(false);		

		// Initialize the sender, this will automatically start rendering output via NDI
		ActiveViewportSender->Initialize();

		// We've initialized the active viewport
		bIsBroadcastingActiveViewport = true;

		// However we need to update the 'Video Texture' to the active viewport back buffer...
		FSlateApplication::Get().GetRenderer()->OnPreResizeWindowBackBuffer().AddRaw(this, &FNDIConnectionService::OnActiveViewportBackbufferResized);
		FSlateApplication::Get().GetRenderer()->OnBackBufferReadyToPresent().AddRaw(this, &FNDIConnectionService::OnActiveViewportBackbufferReadyToPresent);		
	}

	// always return true
	return true;
}

// Handler for when the active viewport back buffer has been resized
void FNDIConnectionService::OnActiveViewportBackbufferResized(void* Backbuffer)
{
	// Ensure we have a valid video texture
	if (IsValid(VideoTexture))
	{
		// Default to 240p
		static int32 DefaultWidth = 352;
		static int32 DefaultHeight = 240;

		// Set the default video texture to reference nothing
		TRefCountPtr<FRHITexture2D> ShaderTexture2D;
		TRefCountPtr<FRHITexture2D> RenderableTexture;
		FRHIResourceCreateInfo CreateInfo = { FClearValueBinding(FLinearColor(0.0f, 0.0f, 0.0f)) };

		RHICreateTargetableShaderResource2D(
			DefaultWidth,
			DefaultHeight,
			PF_B8G8R8A8,
			1,
			TexCreate_None,
			TexCreate_RenderTargetable,
			false,
			CreateInfo,
			RenderableTexture,
			ShaderTexture2D
		);
		
		VideoTexture->Resource->TextureRHI = (FTextureRHIRef&)RenderableTexture;
		VideoTexture->UpdateResource();
	}
}

// Handler for when the back buffer is read to present to the end user
void FNDIConnectionService::OnActiveViewportBackbufferReadyToPresent(SWindow& Window, const FTexture2DRHIRef& Backbuffer)
{
	if (Window.GetType() == EWindowType::GameWindow || (Window.IsRegularWindow() && IsRunningInEditor()))
	{
		if (IsValid(VideoTexture) && VideoTexture->Resource)
		{
			// Lets improve the performance a bit
			if (VideoTexture->Resource->TextureRHI != Backbuffer)
			{
				VideoTexture->Resource->TextureRHI = (FTexture2DRHIRef&)Backbuffer;
				RHIUpdateTextureReference(VideoTexture->TextureReference.TextureReferenceRHI, Backbuffer);
			}
		}
	}
}

void FNDIConnectionService::StopBroadcastingActiveViewport()
{
	// Wait for the sync context locks
	FScopeLock RenderLock(&RenderSyncContext);

	// reset the initialization properties
	IsInEditorMode = false;

	// Ensure that if the active viewport sender is active, that we shut it down
	if (IsValid(this->ActiveViewportSender))
	{
		// shutdown the active viewport sender (just in case it was activated)
		this->ActiveViewportSender->Shutdown();

		// reset the broadcasting flag, so that we can restart the broadcast later
		this->bIsBroadcastingActiveViewport = false;
	}
}