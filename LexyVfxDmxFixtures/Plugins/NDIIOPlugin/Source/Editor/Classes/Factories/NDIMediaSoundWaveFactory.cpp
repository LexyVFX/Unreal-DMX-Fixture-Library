/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#include <Factories/NDIMediaSoundWaveFactory.h>

#include <AssetTypeCategories.h>
#include <Objects/Media/NDIMediaSoundWave.h>

#define LOCTEXT_NAMESPACE "NDIIOEditorMediaSoundWaveFactory"

UNDIMediaSoundWaveFactory::UNDIMediaSoundWaveFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {

	this->bCreateNew = true;
	this->bEditAfterNew = true;

	this->SupportedClass = UNDIMediaSoundWave::StaticClass();
}

FText UNDIMediaSoundWaveFactory::GetDisplayName() const { return LOCTEXT("NDIMediaSoundWaveFactoryDisplayName", "NDI Media Sound Wave"); }

uint32 UNDIMediaSoundWaveFactory::GetMenuCategories() const { return EAssetTypeCategories::Sounds; }

UObject* UNDIMediaSoundWaveFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UNDIMediaSoundWave>(InParent, InName, Flags | RF_Transactional);
}

#undef LOCTEXT_NAMESPACE