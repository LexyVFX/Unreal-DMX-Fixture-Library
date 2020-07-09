/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#include <Objects/Media/NDIMediaReceiver.h>
#include <Misc/CoreDelegates.h>
#include <RenderTargetPool.h>
#include <GlobalShader.h>
#include <ShaderParameterUtils.h>
#include <MediaShaders.h>

#if WITH_EDITOR
	#include <Editor.h>
#endif

UNDIMediaReceiver::UNDIMediaReceiver(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {

}

/**
	Attempts to perform initialization logic for creating a receiver through the NDI® sdk api
*/
bool UNDIMediaReceiver::Initialize(const FNDIConnectionInformation& InConnectionInformation)
{
	if (this->p_receive_instance == nullptr)
	{
		// create a non-connected receiver instance
		NDIlib_recv_create_v3_t settings;
		settings.allow_video_fields = false;
		settings.bandwidth = NDIlib_recv_bandwidth_highest;
		settings.color_format = NDIlib_recv_color_format_e_BGRX_BGRA;
		
		p_receive_instance = NDIlib_recv_create_v3(&settings);

		// check if it was successful
		if (p_receive_instance != nullptr) 
		{
			// If the user has added an audio wave to this object, ensure that we register ourselves with
			// the object
			if (this->AudioWave != nullptr)
			{
				// Register ourselves with the AudioWave object, so that when the engine calls
				// GeneratePCMData on the audio wave, it can be routed to our implementation
				AudioWave->SetConnectionSource(this);
			}
			
			//// Alright we created a non-connected receiver. Lets actually connect
			ChangeConnection(InConnectionInformation);			

			// We don't want to limit the engine rendering speed to the sync rate of the connection hook 
			// into the core delegates render thread 'EndFrame'
			FCoreDelegates::OnEndFrameRT.AddUObject(this, &UNDIMediaReceiver::CaptureConnectedVideo);

			#if UE_EDITOR

			// We don't want to provide perceived issues with the plugin not working so
			// when we get a Pre-exit message, forcefully shutdown the receiver
			FCoreDelegates::OnPreExit.AddWeakLambda( this, [&]() { 
				this->Shutdown(); 
				FCoreDelegates::OnPreExit.RemoveAll(this);
			});

			// We handle this in the 'Play In Editor' versions as well.
			FEditorDelegates::PrePIEEnded.AddWeakLambda( this, [&](const bool) { 
				this->Shutdown(); 
				FEditorDelegates::PrePIEEnded.RemoveAll(this);
			});

			#endif

			return true;
		}

	}

	return false;
}

/**
	Attempts to change the connection to another NDI® sender source
*/
void UNDIMediaReceiver::ChangeConnection(const FNDIConnectionInformation& InConnectionInformation)
{
	// Ensure some thread-safety because our 'Capture Connected Video' function is called on the render thread
	FScopeLock Lock(&RenderSyncContext);

	if (p_receive_instance != nullptr)
	{
		// Ensure that the connection information is valid
		if (InConnectionInformation.IsValid())
		{
			// Do the implicit conversion on the connection information
			NDIlib_source_t connection;
			connection.p_url_address = TCHAR_TO_UTF8(*InConnectionInformation.Url);
			connection.p_ndi_name = TCHAR_TO_UTF8(*InConnectionInformation.SourceName);

			// connect to the source 		
			NDIlib_recv_connect(p_receive_instance, &connection);

			// Copy the InConnection structure to our ConnectionInformation
			this->ConnectionInformation = InConnectionInformation;
		}

		// otherwise just set the connection to 'null' so that we can still keep the receiver
		else
		{
			NDIlib_recv_connect(p_receive_instance, nullptr);
			this->ConnectionInformation.Reset();
		}

		// destroy the previous framesync_instance
		if (p_framesync_instance != nullptr)
			NDIlib_framesync_destroy(p_framesync_instance);

		// create a new frame sync instance
		p_framesync_instance = NDIlib_framesync_create(p_receive_instance);
	}
}

/**
	Attempts to change the SoundWave object used as the audio frame capture object
*/
void UNDIMediaReceiver::ChangeAudioWave(UNDIMediaSoundWave* InAudioWave)
{
	FScopeLock Lock(&AudioSyncContext);

	// Remove ourselves from the previous audio wave object
	if (IsValid(this->AudioWave))
	{
		this->AudioWave->SetConnectionSource(nullptr);
	}

	// set to the new wave object
	this->AudioWave = InAudioWave;

	// ensure we are registered with the audio source, so that we can be called
	// in place of 'GeneratePCMData'
	if (IsValid(AudioWave))
	{
		this->AudioWave->SetConnectionSource(nullptr);
	}
}

/**
	Attempts to change the Video Texture object used as the video frame capture object
*/
void UNDIMediaReceiver::ChangeVideoTexture(UNDIMediaTexture2D* InVideoTexture)
{
	FScopeLock Lock(&RenderSyncContext);

	if (IsValid(this->VideoTexture))
	{
		// make sure that the old texture is not referencing the rendering of this texture
		this->VideoTexture->UpdateTextureReference(FRHICommandListExecutor::GetImmediateCommandList(), nullptr);
	}

	// Just copy the new texture here.
	this->VideoTexture = InVideoTexture;
}

/**
	Attempts to generate the pcm data required by the 'AudioWave' object
*/
int32 UNDIMediaReceiver::GeneratePCMData(uint8* PCMData, const int32 SamplesNeeded)
{
	int32 samples_generated = 0;
	int32 sample_rate	= IsValid(AudioWave) ? AudioWave->GetSampleRateForCurrentPlatform() : 48000;
	int32 channels		= IsValid(AudioWave) ? AudioWave->NumChannels : 1;

	if (p_framesync_instance != nullptr)
	{
		NDIlib_audio_frame_v2_t audio_frame;
		NDIlib_framesync_capture_audio(p_framesync_instance, &audio_frame, sample_rate, channels, SamplesNeeded / channels);

		const int32 available_samples = audio_frame.no_samples*channels;
		
		for (int32 iter = 0; iter < available_samples; ++iter)
		{
			// convert float to int16
			int32 index_sample = FMath::RoundToInt(audio_frame.p_data[iter] * 32767.0f);

			// perform clamp between different integer types
			int16 sample =
				index_sample < INT16_MIN ? INT16_MIN :
				index_sample > INT16_MAX ? INT16_MAX :
				index_sample;

			PCMData[iter * 2] = sample & 0xff;
			PCMData[iter * 2 + 1] = (sample >> 8) & 0xff;
		}

		// clean up our audio frame
		NDIlib_framesync_free_audio(p_framesync_instance, &audio_frame);

		samples_generated = available_samples*sizeof(int16);
	}

	return samples_generated;
}

/**
	This will add a metadata frame and return immediately, having scheduled the frame asynchronously
*/
void UNDIMediaReceiver::SendMetadataFrame()
{
	// Currently unsupported
}

/**
	This will set the up-stream tally notifications. If no streams are connected, it will automatically
	send the tally state upon connection
*/
void UNDIMediaReceiver::SendTallyInformation(const bool& IsOnPreview, const bool& IsOnProgram)
{
	// Currently unsupported
}

/**
	Attempts to immediately stop receiving frames from the connected NDI sender
*/
void UNDIMediaReceiver::Shutdown()
{
	{ FScopeLock AudioLock(&AudioSyncContext);
		
		if (IsValid(AudioWave))
		{
			// Remove ourselves from the Audio wave object which is trying to render audio frames
			// as fast as possible
			this->AudioWave->SetConnectionSource(nullptr);
		}
	}

	{ FScopeLock RenderLock(&RenderSyncContext);

		if (p_receive_instance != nullptr)
		{
			if (p_framesync_instance != nullptr)
			{
				NDIlib_framesync_destroy(p_framesync_instance);
				p_framesync_instance = nullptr;
			}

			NDIlib_recv_destroy(p_receive_instance);
			p_receive_instance = nullptr;
		}
	}

	this->ConnectionInformation.Reset();
	this->PerformanceData.Reset();
	this->FrameRate = FFrameRate(60, 1);
	this->Timecode = FTimecode(0, FrameRate, true, true);	
}

/**
   Called before destroying the object.  This is called immediately upon deciding to destroy the object,
   to allow the object to begin an asynchronous cleanup process.
 */
void UNDIMediaReceiver::BeginDestroy()
{
	// Call the shutdown procedure here.
	this->Shutdown();

	// Call the base implementation of 'BeginDestroy'
	Super::BeginDestroy();	
}

/**
	Attempts to immediately update the 'VideoTexture' object with the last capture video frame
	from the connected source
*/
void UNDIMediaReceiver::CaptureConnectedVideo()
{
	// This function is called on the Engine's Main Rendering Thread. Be very careful when doing stuff here.
	// Make sure things are done quick and efficient.

	// check for our frame sync object and that we are actually connected to the end point
	if (p_framesync_instance != nullptr)
	{
		// Ensure thread safety
		FScopeLock Lock(&RenderSyncContext);

		// Ensure we have something to render on to
		if (IsValid(this->VideoTexture) && VideoTexture->Resource != nullptr)
		{
			// Using a frame-sync we can always get data which is the magic and it will adapt 
			// to the frame-rate that it is being called with.
			NDIlib_video_frame_v2_t video_frame;			
			NDIlib_framesync_capture_video(p_framesync_instance, &video_frame, NDIlib_frame_format_type_progressive);
			
			// Display video here. The reason that the frame-sync does not return a frame until it has
			// received the frame (e.g. it could return a black 1920x1080 image p) is that you are likely to
			// want to default to some video standard (NTSC or PAL) and there would be no way to know what
			// your default image should be from an API level.
			if (video_frame.p_data)
			{
				FRHICommandListImmediate& RHICmdList = FRHICommandListExecutor::GetImmediateCommandList();
				
				// Actually draw the video frame from cpu to gpu
				DrawVideoFrame(RHICmdList, video_frame);				
			}

			// Release the video. You could keep the frame if you want and release it later.
			NDIlib_framesync_free_video(p_framesync_instance, &video_frame);		
			
			// Update our Performance Metrics
			GatherPerformanceMetrics();
		}
	}
}

/**
	Perform the color conversion (if any) and bit copy from the gpu
*/
void UNDIMediaReceiver::DrawVideoFrame(FRHICommandListImmediate& RHICmdList, NDIlib_video_frame_v2_t& Result)
{
	// Initialize the frame size parameter
	FIntPoint FrameSize = FIntPoint(Result.xres, Result.yres);

	if (!RenderTargetDescriptor.IsValid() || RenderTargetDescriptor.GetSize() != FIntVector(Result.xres, Result.yres, 0))
	{	
		FRHIResourceCreateInfo CreateInfo;
		EPixelFormat PixelFormat = PF_B8G8R8A8;

		// Create the RenderTarget descriptor
		RenderTargetDescriptor = FPooledRenderTargetDesc::Create2DDesc(
			FrameSize,
			PixelFormat,
			FClearValueBinding::None,
			TexCreate_None,
			TexCreate_RenderTargetable,
			false
		);

		// Create a temporary targetable texture
		FTexture2DRHIRef InputTarget = RHICreateTexture2D(
			FrameSize.X, FrameSize.Y,
			PixelFormat, 1, 1,
			TexCreate_CPUReadback,
			CreateInfo
		);

		// Update the shader resource for the 'SourceTexture'
		RHICreateTargetableShaderResource2D(
			FrameSize.X, FrameSize.Y,
			PixelFormat,
			1,
			TexCreate_Dynamic,
			TexCreate_RenderTargetable,
			false,
			CreateInfo,
			InputTarget,
			SourceTexture
		);

		// Update the shader resource for the 'ConversionTexture'
		RHICreateTargetableShaderResource2D(
			FrameSize.X, FrameSize.Y,
			PixelFormat,
			1,
			TexCreate_Dynamic,
			TexCreate_RenderTargetable,
			false,
			CreateInfo,
			InputTarget,
			ConversionTexture
		);
	}

	// Initialize the Graphics Pipeline State Object
	FGraphicsPipelineStateInitializer GraphicsPSOInit;

	// Initialize the Render pass with the conversion texture
	FRHIRenderPassInfo RPInfo(ConversionTexture, ERenderTargetActions::DontLoad_Store);

	// do as it suggests
	RHICmdList.ApplyCachedRenderTargets(GraphicsPSOInit);

	// set the state objects
	GraphicsPSOInit.DepthStencilState = TStaticDepthStencilState<false, CF_Always>::GetRHI();
	GraphicsPSOInit.RasterizerState = TStaticRasterizerState<>::GetRHI();
	GraphicsPSOInit.BlendState = TStaticBlendStateWriteMask<CW_RGBA, CW_NONE, CW_NONE, CW_NONE, CW_NONE, CW_NONE, CW_NONE, CW_NONE>::GetRHI();
	GraphicsPSOInit.PrimitiveType = PT_TriangleStrip;

	// configure media shaders
	//TShaderMap<FGlobalShaderType>* ShaderMap = GetGlobalShaderMap(GMaxRHIFeatureLevel);
	FVertexBufferRHIRef VertexBuffer = CreateTempMediaVertexBuffer();

	// construct the shaders
	TShaderMapRef<FMediaShadersVS> VertexShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));
	TShaderMapRef<FRGBConvertPS> ConvertShader(GetGlobalShaderMap(GMaxRHIFeatureLevel));

	// perform binding operations for the shaders to be used
	GraphicsPSOInit.BoundShaderState.VertexDeclarationRHI = GMediaVertexDeclaration.VertexDeclarationRHI;
	GraphicsPSOInit.BoundShaderState.VertexShaderRHI = VertexShader.GetVertexShader();
	GraphicsPSOInit.BoundShaderState.PixelShaderRHI = ConvertShader.GetPixelShader();

	// Ensure the pipeline state is set to the one we've configured
	SetGraphicsPipelineState(RHICmdList, GraphicsPSOInit);

	// Create the update region structure
	FUpdateTextureRegion2D Region(0, 0, 0, 0, FrameSize.X, FrameSize.Y);

	// Set the Pixel data of the NDI Frame to the SourceTexture
	RHIUpdateTexture2D(SourceTexture, 0, Region, Result.line_stride_in_bytes, (uint8*&)Result.p_data);

	// set the texture parameter of the conversion shader
	ConvertShader->SetParameters(RHICmdList, SourceTexture, FrameSize, bPerformsRGBtoLinear);

	// set the stream source
	RHICmdList.SetStreamSource(0, VertexBuffer, 0);

	// begin our drawing
	{
		RHICmdList.BeginRenderPass(RPInfo, TEXT("NDI Recv Color Conversion"));

		RHICmdList.SetViewport(0, 0, 0.0f, FrameSize.X, FrameSize.Y, 1.0f);
		RHICmdList.DrawPrimitive(0, 2, 1);

		RHICmdList.EndRenderPass();
	}

	this->VideoTexture->UpdateTextureReference(RHICmdList, (FTexture2DRHIRef&)ConversionTexture);
}

/**
	Attempts to gather the performance metrics of the connection to the remote source
*/
void UNDIMediaReceiver::GatherPerformanceMetrics()
{
	// provide references to store the values
	NDIlib_recv_performance_t stable_performance;
	NDIlib_recv_performance_t dropped_performance;

	// get the performance values from the SDK
	NDIlib_recv_get_performance(p_receive_instance, &stable_performance, &dropped_performance);

	// update our structure with the updated values
	this->PerformanceData.AudioFrames = stable_performance.audio_frames;
	this->PerformanceData.DroppedAudioFrames = dropped_performance.audio_frames;
	this->PerformanceData.DroppedMetadataFrames = dropped_performance.metadata_frames;
	this->PerformanceData.DroppedVideoFrames = dropped_performance.video_frames;
	this->PerformanceData.MetadataFrames = stable_performance.metadata_frames;
	this->PerformanceData.VideoFrames = stable_performance.video_frames;
}

/**
	Returns the sound wave object used by this object for audio
*/
UNDIMediaSoundWave* UNDIMediaReceiver::GetMediaSoundWave() const { return this->AudioWave; }

/**
	Returns the current performance data of the receiver while connected to the source
*/
const FNDIReceiverPerformanceData& UNDIMediaReceiver::GetPerformanceData() const { return this->PerformanceData; }

/**
	Returns the current connection information of the connected source
*/
const FNDIConnectionInformation& UNDIMediaReceiver::GetCurrentConnectionInformation() const { return this->ConnectionInformation; }

/**
	Returns the current timecode of the connected source
*/
const FTimecode& UNDIMediaReceiver::GetCurrentTimecode() const { return this->Timecode; }

/**
	Set whether or not a RGB to Linear conversion is made
*/
void UNDIMediaReceiver::PerformsRGBToLinearConversion(bool Value) { this->bPerformsRGBtoLinear = Value; }

/**
	Returns the current framerate of the connected source
*/
const FFrameRate& UNDIMediaReceiver::GetCurrentFrameRate() const { return this->FrameRate; }
