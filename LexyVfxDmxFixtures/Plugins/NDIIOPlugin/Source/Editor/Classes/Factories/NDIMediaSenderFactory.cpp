/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#include <Factories/NDIMediaSenderFactory.h>

#include <AssetTypeCategories.h>
#include <Objects/Media/NDIMediaSender.h>

#define LOCTEXT_NAMESPACE "NDIIOEditorMediaSenderFactory"

UNDIMediaSenderFactory::UNDIMediaSenderFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {

	bCreateNew = true;
	bEditAfterNew = true;

	this->SupportedClass = UNDIMediaSender::StaticClass();
}

FText UNDIMediaSenderFactory::GetDisplayName() const { return LOCTEXT("NDIMediaSenderFactoryDisplayName", "NDI Media Sender"); }

uint32 UNDIMediaSenderFactory::GetMenuCategories() const { return EAssetTypeCategories::Media; }

UObject* UNDIMediaSenderFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UNDIMediaSender>(InParent, InName, Flags | RF_Transactional);
}

#undef LOCTEXT_NAMESPACE