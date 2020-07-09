/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#pragma once

#include <CoreMinimal.h>

#include <Components/ActorComponent.h>
#include <Structures/NDIConnectionInformation.h>

#include "NDIFinderComponent.generated.h"

/** Delegates **/

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNDIFinderServiceCollectionChangedDelegate, UNDIFinderComponent*, InComponent);

/** ******************* **/

/** 
	A component used for essential functionality when dealing with the finder service. Allowing you to 
	get a collection of sources found on the network.
*/
UCLASS(BlueprintType, Blueprintable, Category = "NDI IO", META = (DisplayName = "NDI Finder Component", BlueprintSpawnableComponent))
class NDIIO_API UNDIFinderComponent : public UActorComponent
{
	GENERATED_UCLASS_BODY()

	public:
		/** A collection of the current sources and their information, found on the network */
		UPROPERTY()
		TArray<FNDIConnectionInformation> NetworkSourceCollection;

		/** A delegate which is broadcast when any change to the network source collection has been detected */
		UPROPERTY(BlueprintAssignable, META = (DisplayName = "On Network Sources Changed", AllowPrivateAccess = true))
		FNDIFinderServiceCollectionChangedDelegate OnNetworkSourcesChanged;
		
	public:
		/** 
			Attempts to find a network source by the supplied name.

			@param ConnectionInformation An existing source information structure which contains the source name
			@param InSourceName A string value representing the name of the source to find
			@result A value indicating whether a source with the supplied name was found
		*/
		UFUNCTION(BlueprintCallable, Category = "NDI IO", META = (DisplayName = "Find Network Source by Name"))
		const bool FindNetworkSourceByName(FNDIConnectionInformation& ConnectionInformation, FString InSourceName = FString(""));

		/** 
			Returns the current collection of sources found on the network
		*/
		UFUNCTION(BlueprintCallable, BlueprintPure, Category = "NDI IO", META = (DisplayName = "Get Network Sources"))
		const TArray<FNDIConnectionInformation> GetNetworkSources();

	protected:
		virtual void BeginPlay() override;
		virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

		/** An override function for when the network source collection has been changed */
		UFUNCTION(BlueprintImplementableEvent, META = (DisplayName = "On Network Sources Changed Event"))
		void OnNetworkSourcesChangedEvent();

	private:
		/** 
			An Event handler for when the NDI Finder Service notifies listeners that changes have been
			detected in the network source collection 
		*/
		UFUNCTION()
		virtual void OnNetworkSourceCollectionChangedEvent() final;

	private:
		FCriticalSection CollectionSyncContext;
};