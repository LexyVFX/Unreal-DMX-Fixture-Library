/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#pragma once

#include <CoreMinimal.h>
#include <Engine/Texture.h>

#include "NDIMediaTexture2D.generated.h"

/** 
	A Texture Object used by an NDI Media Receiver object for capturing video from
	a network source
*/
UCLASS(NotBlueprintType, NotBlueprintable, HideDropdown, HideCategories = (ImportSettings, Compression, Texture, Adjustments, Compositing, LevelOfDetail, Object), META = (DisplayName = "NDI Media Texture 2D"))
class NDIIO_API UNDIMediaTexture2D : public UTexture
{
	GENERATED_UCLASS_BODY()

	public:
		virtual float GetSurfaceHeight() const override;
		virtual float GetSurfaceWidth() const override;

		virtual void GetResourceSizeEx(FResourceSizeEx& CumulativeResourceSize) override;
		virtual EMaterialValueType GetMaterialType() const override;

		virtual void UpdateTextureReference(FRHICommandList& RHICmdList, FTexture2DRHIRef Reference) final;

	private:
		virtual class FTextureResource* CreateResource() override;
};