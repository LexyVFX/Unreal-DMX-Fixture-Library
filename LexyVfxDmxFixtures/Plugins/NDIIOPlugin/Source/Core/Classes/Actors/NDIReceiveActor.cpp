/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/


#include <Actors/NDIReceiveActor.h>
#include <Services/NDIConnectionService.h>

#include <AudioDevice.h>
#include <ActiveSound.h>
#include <Async/Async.h>
#include <Engine/StaticMesh.h>
#include <Kismet/GameplayStatics.h>
#include <Materials/MaterialInstanceDynamic.h>
#include <Objects/Media/NDIMediaTexture2D.h>
#include <UObject/ConstructorHelpers.h>

ANDIReceiveActor::ANDIReceiveActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {

	// Get the Engine's 'Plane' static mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh>		 MeshObject(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));	
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialObject(TEXT("Material'/NDIIOPlugin/Materials/NDI_Unlit_SourceMaterial.NDI_Unlit_SourceMaterial'"));

	// Ensure that the object is valid
	if (MeshObject.Object)
	{
		// Create the static mesh component visual
		this->VideoMeshComponent = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("VideoMeshComponent"));

		// setup the attachment and modify the position, rotation, and mesh properties
		this->VideoMeshComponent->SetupAttachment(RootComponent);
		this->VideoMeshComponent->SetStaticMesh(MeshObject.Object);
		this->VideoMeshComponent->SetRelativeRotation(FQuat::MakeFromEuler(FVector(90.0f, 0.0f, 90.0f)));
		this->VideoMeshComponent->SetRelativeScale3D(FVector(FrameWidth / 100.0f, FrameHeight / 100.0f, 1.0f));

		this->VideoMeshComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
		this->VideoMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		this->VideoMeshComponent->SetCollisionObjectType(ECC_WorldDynamic);
		
		// This is object is mainly used for simple tests and things that don't require
		// additional material shading support, store the an unlit source material to display	
		this->VideoMaterial = MaterialObject.Object;

		// If the material is valid
		if (this->VideoMaterial)
		{
			// Set the Mesh Material to the Video Material
			this->VideoMeshComponent->SetMaterial(0, this->VideoMaterial);
		}
	}

	this->AudioComponent = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("AudioComponent"));
	this->AudioComponent->SetupAttachment(RootComponent);
	this->AudioComponent->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator::ZeroRotator);
	this->AudioComponent->SetRelativeScale3D(FVector::OneVector);	

	this->bAllowTickBeforeBeginPlay = false;	
}

void ANDIReceiveActor::BeginPlay()
{
	// call the base implementation for 'BeginPlay'
	Super::BeginPlay();
	
	// We need to validate that we have media source, so we can set the texture in the material instance
	if (IsValid(this->NDIMediaSource))
	{
		// Validate the Video Material Instance so we can set the texture used in the NDI Media source
		if (IsValid(this->VideoMaterial))
		{
			// create and set the instance material from the MaterialObject
			VideoMaterialInstance = this->VideoMeshComponent->CreateAndSetMaterialInstanceDynamicFromMaterial(0, this->VideoMaterial);

			// Ensure we have a valid material instance
			if (IsValid(VideoMaterialInstance))
			{
				// alright ensure that the video texture is always enabled
				this->VideoMaterialInstance->SetScalarParameterValue("Enable Video Alpha", 0.0f);
				this->VideoMaterialInstance->SetScalarParameterValue("Enable Video Texture", 1.0f);	

				this->NDIMediaSource->UpdateMaterialTexture(VideoMaterialInstance, "Video Texture");				
			}
		}

		// Define the basic parameters for constructing temporary audio wave object
		FString AudioSource = FString::Printf(TEXT("AudioSource_%s"), *GetFName().ToString().Right(1));
		FName AudioWaveName = FName(*AudioSource);
		EObjectFlags Flags = RF_Public | RF_Standalone | RF_Transient | RF_MarkAsNative;

		// Construct a temporary audio sound wave to be played by this component
		this->AudioSoundWave = NewObject<UNDIMediaSoundWave>(
			GetTransientPackage(),
			UNDIMediaSoundWave::StaticClass(),
			AudioWaveName,
			Flags
		);

		// Ensure the validity of the temporary sound wave object
		if (IsValid(this->AudioSoundWave))
		{
			// Set the sound of the Audio Component and Ensure playback
			this->AudioComponent->SetSound(this->AudioSoundWave);

			// Ensure we register the audio wave object with the media.
			this->NDIMediaSource->RegisterAudioWave(AudioSoundWave);
		}

		if (this->NDIMediaSource->GetCurrentConnectionInformation().IsValid())
		{
			if (IsValid(AudioComponent))
			{
				// we should play the audio, if we want audio playback
				if (bEnableAudioPlayback)
				{
					this->AudioComponent->Play(0.0f);
				}

				// otherwise just stop
				else this->AudioComponent->Stop();
			}
		}

		// Add a lambda to the OnReceiverConnected Event
		else this->NDIMediaSource->OnNDIReceiverConnectedEvent.AddWeakLambda(this, [&](UNDIMediaReceiver*) {

			// Ensure that the audio component is valid
			if (IsValid(AudioComponent))
			{
				// we should play the audio, if we want audio playback
				if (bEnableAudioPlayback)
				{
					this->AudioComponent->Play(0.0f);
				}

				// otherwise just stop
				else this->AudioComponent->Stop();
			}
		});
	}
}

void ANDIReceiveActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Ensure we have a valid material instance
	if (EndPlayReason == EEndPlayReason::EndPlayInEditor && IsValid(VideoMaterialInstance))
	{
		// alright ensure that the video texture is always enabled
		this->VideoMaterialInstance->SetScalarParameterValue("Enable Video Texture", 0.0f);
	}
}

/**
	Attempts to set the desired frame size in cm, represented in the virtual scene
*/
void ANDIReceiveActor::SetFrameSize(FVector2D InFrameSize)
{
	// clamp the values to the lowest we'll allow
	const float frame_height = FMath::Max(InFrameSize.Y, 0.00001f);
	const float frame_width = FMath::Max(InFrameSize.X, 0.00001f);

	// validate the static mesh component
	if (IsValid(this->VideoMeshComponent))
	{
		// change the scale of the video 
		this->VideoMeshComponent->SetRelativeScale3D(FVector(frame_width / 100.0f, frame_height / 100.0f, 1.0f));
	}
}

void ANDIReceiveActor::SetFrameHeight(const float& InFrameHeight)
{
	// Clamp the Frame Height to a minimal value
	FrameHeight = FMath::Max(InFrameHeight, 0.00001f);

	// Call the function to set the frame size with the newly clamped value
	SetFrameSize(FVector2D(FrameWidth, FrameHeight));
}

void ANDIReceiveActor::SetFrameWidth(const float& InFrameWidth)
{
	// Clamp the Frame Width to a minimal value
	FrameWidth = FMath::Max(InFrameWidth, 0.00001f);

	// Call the function to set the frame size with the newly clamped value
	SetFrameSize(FVector2D(FrameWidth, FrameHeight));
}

void ANDIReceiveActor::UpdateAudioPlayback(const bool& Enabled)
{
	// Ensure validity and we are currently playing
	if (IsValid(this->AudioComponent))
	{
		// Stop playback when possible
		if (Enabled)
		{
			// Start the playback
			this->AudioComponent->Play(0.0f);
		}

		// otherwise just stop playback (even if it's not playing)
		else this->AudioComponent->Stop();
	}
}

/**
	Returns the current frame size of the 'VideoMeshComponent' for this object
*/
const FVector2D ANDIReceiveActor::GetFrameSize() const { return FVector2D(FrameWidth, FrameHeight); }

#if WITH_EDITORONLY_DATA

void ANDIReceiveActor::PreEditChange(FProperty* InProperty)
{
	// call the base class 'PreEditChange'
	Super::PreEditChange(InProperty);
	
}

void ANDIReceiveActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// get the name of the property which changed
	FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	// compare against the 'FrameHeight' property
	if (PropertyName == GET_MEMBER_NAME_CHECKED(ANDIReceiveActor, FrameHeight))
	{
		// resize the frame
		SetFrameSize(FVector2D(FrameWidth / 100.0f, FrameHeight / 100.0f));
	}

	// compare against the 'FrameWidth' property
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(ANDIReceiveActor, FrameWidth))
	{
		// resize the frame
		SetFrameSize(FVector2D(FrameWidth / 100.0f, FrameHeight / 100.0f));
	}

	// compare against the 'bEnableAudioPlayback' property
	else if (PropertyName == GET_MEMBER_NAME_CHECKED(ANDIReceiveActor, bEnableAudioPlayback))
	{
		// resize the frame
		UpdateAudioPlayback(bEnableAudioPlayback);
	}

	// call the base class 'PostEditChangeProperty'
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

#endif
