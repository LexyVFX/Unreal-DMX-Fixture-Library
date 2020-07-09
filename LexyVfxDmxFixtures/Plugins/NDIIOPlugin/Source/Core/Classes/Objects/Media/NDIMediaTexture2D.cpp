/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#include <Objects/Media/NDIMediaTexture2D.h>
#include <Objects/Media/NDIMediaTextureResource.h>

UNDIMediaTexture2D::UNDIMediaTexture2D(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {

	this->Resource = nullptr;
}

void UNDIMediaTexture2D::UpdateTextureReference(FRHICommandList& RHICmdList, FTexture2DRHIRef Reference)
{
	if (Resource != nullptr)
	{
		static int32 DefaultWidth = 1280;
		static int32 DefaultHeight = 720;

		if (Reference.IsValid() && Resource->TextureRHI != Reference)
		{
			Resource->TextureRHI = (FTexture2DRHIRef&)Reference;
			RHIUpdateTextureReference(TextureReference.TextureReferenceRHI, Resource->TextureRHI);			
		}
		else if (!Reference.IsValid())
		{
			if (FNDIMediaTextureResource* TextureResource = static_cast<FNDIMediaTextureResource*>(this->Resource))
			{
				// Set the default video texture to reference nothing
				TRefCountPtr<FRHITexture2D> ShaderTexture2D;
				TRefCountPtr<FRHITexture2D> RenderableTexture;
				FRHIResourceCreateInfo CreateInfo = { FClearValueBinding(FLinearColor(0.0f, 0.0f, 0.0f)) };

				RHICreateTargetableShaderResource2D(
					DefaultWidth,
					DefaultHeight,
					EPixelFormat::PF_B8G8R8A8,
					1,
					TexCreate_Dynamic,
					TexCreate_RenderTargetable,
					false,
					CreateInfo,
					RenderableTexture,
					ShaderTexture2D
				);

				TextureResource->TextureRHI = (FTextureRHIRef&)RenderableTexture;

				ENQUEUE_RENDER_COMMAND(FNDIMediaTexture2DUpdateTextureReference)(
					[&](FRHICommandListImmediate& RHICmdList) {

					RHIUpdateTextureReference(TextureReference.TextureReferenceRHI, TextureResource->TextureRHI);
				});

				// Make sure _RenderThread is executed before continuing
				FlushRenderingCommands();
			}
		}
	}
}

FTextureResource* UNDIMediaTexture2D::CreateResource()
{
	if (this->Resource != nullptr)
	{
		delete Resource;
		Resource = nullptr;
	}

	if (FNDIMediaTextureResource* TextureResource = new FNDIMediaTextureResource(this))
	{
		this->Resource = TextureResource;
		
		static int32 DefaultWidth = 1280;
		static int32 DefaultHeight = 720;

		// Set the default video texture to reference nothing
		TRefCountPtr<FRHITexture2D> ShaderTexture2D;
		TRefCountPtr<FRHITexture2D> RenderableTexture;
		FRHIResourceCreateInfo CreateInfo = { FClearValueBinding(FLinearColor(0.0f, 0.0f, 0.0f)) };

		RHICreateTargetableShaderResource2D(
			DefaultWidth,
			DefaultHeight,
			EPixelFormat::PF_B8G8R8A8,
			1,
			TexCreate_Dynamic,
			TexCreate_RenderTargetable,
			false,
			CreateInfo,
			RenderableTexture,
			ShaderTexture2D
		);

		Resource->TextureRHI = (FTextureRHIRef&)RenderableTexture;
	}

	return this->Resource;
}

void UNDIMediaTexture2D::GetResourceSizeEx(FResourceSizeEx& CumulativeResourceSize)
{
	Super::GetResourceSizeEx(CumulativeResourceSize);

	if (FNDIMediaTextureResource* CurrentResource = static_cast<FNDIMediaTextureResource*>(this->Resource))
	{
		CumulativeResourceSize.AddUnknownMemoryBytes(CurrentResource->GetResourceSize());
	}	
}

float UNDIMediaTexture2D::GetSurfaceHeight() const { return Resource != nullptr ? Resource->GetSizeY() : 0.0f; }

float UNDIMediaTexture2D::GetSurfaceWidth() const { return Resource != nullptr ? Resource->GetSizeX() : 0.0f; }

EMaterialValueType UNDIMediaTexture2D::GetMaterialType() const { return MCT_Texture2D; }
