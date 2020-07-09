/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#include <Factories/NDIMediaTexture2DFactory.h>

#include <AssetTypeCategories.h>
#include <Objects/Media/NDIMediaTexture2D.h>

#define LOCTEXT_NAMESPACE "NDIIOEditorMediaSoundWaveFactory"

UNDIMediaTexture2DFactory::UNDIMediaTexture2DFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {

	this->bCreateNew = true;
	this->bEditAfterNew = true;

	this->SupportedClass = UNDIMediaTexture2D::StaticClass();
}

FText UNDIMediaTexture2DFactory::GetDisplayName() const { return LOCTEXT("NDIMediaTexture2DFactoryDisplayName", "NDI Media Texture2D"); }

uint32 UNDIMediaTexture2DFactory::GetMenuCategories() const { return EAssetTypeCategories::MaterialsAndTextures; }

UObject* UNDIMediaTexture2DFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	if (UNDIMediaTexture2D* Resource = NewObject<UNDIMediaTexture2D>(InParent, InName, Flags | RF_Transactional))
	{
		Resource->UpdateResource();
		return Resource;
	}

	return nullptr;
}

#undef LOCTEXT_NAMESPACE