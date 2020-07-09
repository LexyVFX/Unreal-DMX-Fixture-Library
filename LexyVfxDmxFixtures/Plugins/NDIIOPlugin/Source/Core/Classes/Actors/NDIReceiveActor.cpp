/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/


#include <Actors/NDIReceiveActor.h>

#include <Engine/StaticMesh.h>
#include <UObject/ConstructorHelpers.h>
#include <Objects/Media/NDIMediaTexture2D.h>

ANDIReceiveActor::ANDIReceiveActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {

	// Get the Engine's 'Plane' static mesh
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshObject(TEXT("StaticMesh'/Engine/BasicShapes/Plane.Plane'"));	

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
	}

	// create the audio component used for audio playback
	this->AudioComponent = ObjectInitializer.CreateDefaultSubobject<UAudioComponent>(this, TEXT("AudioComponent"));
	this->AudioComponent->SetupAttachment(RootComponent);
}

void ANDIReceiveActor::BeginPlay()
{
	// call the base implementation for 'BeginPlay'
	Super::BeginPlay();

	// Validate the NDI Media Source
	if (IsValid(this->NDIMediaSource))
	{
		// Validate the MediaSource Audio object
		if (USoundBase* MediaAudio = NDIMediaSource->GetMediaSoundWave())
		{
			// Set the Sound object of the Media Source and begin playing
			this->AudioComponent->SetSound(MediaAudio);
			this->AudioComponent->Play(0.0f);
		}
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

	// call the base class 'PostEditChangeProperty'
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

#endif
