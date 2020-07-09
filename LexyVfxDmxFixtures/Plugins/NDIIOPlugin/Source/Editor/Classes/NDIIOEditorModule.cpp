/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#include <NDIIOEditorModule.h>

#include <Editor.h>
#include <IPlacementModeModule.h>
#include <Interfaces/IPluginManager.h>
#include <Styling/SlateStyleRegistry.h>

#include <Actors/NDIReceiveActor.h>
#include <Actors/NDIBroadcastActor.h>

#include <FrameWork/Application/SlateApplication.h>

#define LOCTEXT_NAMESPACE "FNDIEditorModule"
#define IMAGE_BRUSH(RelativePath, ...) FSlateImageBrush(StyleInstance->RootToContentDir(RelativePath, TEXT(".png")), __VA_ARGS__)

#define PLACEMENT_CATEGORY TEXT("NDI®")
#define PLACEMENT_LOCTEXT NSLOCTEXT("NewTek", "NDI", "NDI®")
#define PLACEMENT_TEXT TEXT("PMNDI")

void FNDIIOEditorModule::StartupModule()
{
	const FName& CategoryName = PLACEMENT_CATEGORY;
	IPlacementModeModule& PlacementModeModule = IPlacementModeModule::Get();

	const FVector2D Icon20x20(20.0f, 20.0f);
	const FVector2D Icon64x64(64.0f, 64.0f);

	this->StyleInstance = MakeUnique<FSlateStyleSet>("NDIEditorStyle");

	if (IPlugin* NDIIOPlugin = IPluginManager::Get().FindPlugin("NDIIOPlugin").Get())
	{
		StyleInstance->SetContentRoot(FPaths::Combine(NDIIOPlugin->GetContentDir(), TEXT("Editor/Icons")));

		StyleInstance->Set("ClassThumbnail.NDIBroadcastActor",	new IMAGE_BRUSH("NDIBroadcastActorIcon_x64", Icon64x64));
		StyleInstance->Set("ClassIcon.NDIBroadcastActor",		new IMAGE_BRUSH("NDIBroadcastActorIcon_x20", Icon20x20));

		StyleInstance->Set("ClassThumbnail.NDIReceiveActor",	new IMAGE_BRUSH("NDIReceiveActorIcon_x64", Icon64x64));
		StyleInstance->Set("ClassIcon.NDIReceiveActor",			new IMAGE_BRUSH("NDIReceiveActorIcon_x20", Icon20x20));

		StyleInstance->Set("ClassThumbnail.NDIMediaReceiver",	new IMAGE_BRUSH("NDIReceiverIcon_x64", Icon64x64));
		StyleInstance->Set("ClassIcon.NDIMediaReceiver",		new IMAGE_BRUSH("NDIReceiverIcon_x20", Icon20x20));

		StyleInstance->Set("ClassThumbnail.NDIMediaSender",		new IMAGE_BRUSH("NDISenderIcon_x64", Icon64x64));
		StyleInstance->Set("ClassIcon.NDIMediaSender",			new IMAGE_BRUSH("NDISenderIcon_x20", Icon20x20));

		StyleInstance->Set("ClassThumbnail.NDIMediaSoundWave",	new IMAGE_BRUSH("NDISoundWaveIcon_x64", Icon64x64));
		StyleInstance->Set("ClassIcon.NDIMediaSoundWave",		new IMAGE_BRUSH("NDISoundWaveIcon_x20", Icon20x20));

		StyleInstance->Set("ClassThumbnail.NDIMediaTexture2D",	new IMAGE_BRUSH("NDIVideoTextureIcon_x64", Icon64x64));
		StyleInstance->Set("ClassIcon.NDIMediaTexture2D",		new IMAGE_BRUSH("NDIVideoTextureIcon_x20", Icon20x20));

		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance.Get());

		PlacementModeModule.RegisterPlacementCategory(
			FPlacementCategoryInfo(
				PLACEMENT_LOCTEXT,
				CategoryName,
				PLACEMENT_TEXT,
				41, // FBuiltInPlacementCategories::Volumes() == 40
				true
			)
		);
	}

	// Get the Registered Placement Category
	if (const FPlacementCategoryInfo* PlacementCategoryInformation = PlacementModeModule.GetRegisteredPlacementCategory(CategoryName))
	{
		// Register the NDI Broadcast Actor a placeable item within the editor
		PlacementModeModule.RegisterPlaceableItem(PlacementCategoryInformation->UniqueHandle, MakeShareable(
			new FPlaceableItem(
				*UActorFactory::StaticClass(),
				FAssetData(ANDIBroadcastActor::StaticClass()->ClassDefaultObject),
				FName("ClassThumbnail.NDIBroadcastActor"),
				TOptional<FLinearColor>(),
				10,
				NSLOCTEXT("NewTek", "NDIBroadcastActor", "NDI Broadcast Actor")
			))
		);

		// Register the NDI Receive Actor a placeable item within the editor
		PlacementModeModule.RegisterPlaceableItem(PlacementCategoryInformation->UniqueHandle, MakeShareable(
			new FPlaceableItem(
				*UActorFactory::StaticClass(),
				FAssetData(ANDIReceiveActor::StaticClass()->ClassDefaultObject),
				FName("ClassThumbnail.NDIReceiveActor"),
				TOptional<FLinearColor>(),
				20,
				NSLOCTEXT("NewTek", "NDIReceiveActor", "NDI Receive Actor")
			))
		);
	}
}

void FNDIIOEditorModule::ShutdownModule()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance.Get());
	StyleInstance.Reset();

	IPlacementModeModule& PlacementModeModule = IPlacementModeModule::Get();
	PlacementModeModule.UnregisterPlacementCategory(PLACEMENT_CATEGORY);
}

#undef PLACEMENT_CATEGORY
#undef PLACEMENT_LOCTEXT
#undef PLACEMENT_TEXT
#undef IMAGE_BRUSH
#undef LOCTEXT_NAMESPACE