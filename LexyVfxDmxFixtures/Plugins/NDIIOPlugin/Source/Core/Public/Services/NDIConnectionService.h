/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#pragma once

#include <CoreMinimal.h>
#include <Engine/TextureRenderTarget2D.h>

DECLARE_EVENT_OneParam(FNDICoreDelegates, FNDIConnectionServiceSendEvent, int64)

/** 
	A service which runs and triggers updates for interested parties to be notified of 
	Audio and Video Frame events
*/
class NDIIO_API FNDIConnectionService final
{
	public:
		static bool IsInEditorMode;

		static FNDIConnectionServiceSendEvent EventOnSendAudioFrame;
		static FNDIConnectionServiceSendEvent EventOnSendVideoFrame;

	public:
		/** 
			Constructs a new instance of this object 
		*/
		FNDIConnectionService();

		// Begin the service
		bool Start();

		// Stop the service
		void Shutdown();

		bool BeginBroadcastingActiveViewport();
		void StopBroadcastingActiveViewport();

		bool IsRunningInEditor() { return IsInEditorMode; }

	private:
		// Handler for when the Non render thread frame has ended
		void OnEndFrame();

		// Handler for when the render thread frame has ended
		void OnEndRenderFrame();

		// Handler for when the active viewport back buffer has been resized
		void OnActiveViewportBackbufferResized(void* Backbuffer);

		// Handler for when the back buffer is read to present to the end user
		void OnActiveViewportBackbufferReadyToPresent(SWindow& Window, const FTexture2DRHIRef& Backbuffer);

	private:		
		bool bIsInitialized = false;
		bool bIsBroadcastingActiveViewport = false;

		FCriticalSection AudioSyncContext;
		FCriticalSection RenderSyncContext;
		
		UTextureRenderTarget2D* VideoTexture = nullptr;
		class UNDIMediaSender* ActiveViewportSender = nullptr;		
};