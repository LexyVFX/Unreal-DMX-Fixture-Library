/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#include <Components/NDIFinderComponent.h>
#include <Services/NDIFinderService.h>

UNDIFinderComponent::UNDIFinderComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer) {


}

void UNDIFinderComponent::BeginPlay()
{
	Super::BeginPlay();

	// Provide some sense of thread-safety
	FScopeLock Lock(&CollectionSyncContext);

	// Update the NetworkSourceCollection with some sources which that the service has already found
	FNDIFinderService::UpdateSourceCollection(NetworkSourceCollection);

	// Ensure that we are subscribed to the collection changed notification so we can handle it locally
	FNDIFinderService::EventOnNDISourceCollectionChanged.AddUObject(this, &UNDIFinderComponent::OnNetworkSourceCollectionChangedEvent);
}

void UNDIFinderComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// Provide some sense of thread-safety
	FScopeLock Lock(&CollectionSyncContext);

	// Empty the source collection
	this->NetworkSourceCollection.Empty(0);

	// Ensure that we are no longer subscribed to collection change notifications
	FNDIFinderService::EventOnNDISourceCollectionChanged.RemoveAll(this);
}

/**
	An Event handler for when the NDI Finder Service notifies listeners that changes have been
	detected in the network source collection
*/
void UNDIFinderComponent::OnNetworkSourceCollectionChangedEvent()
{
	// Since we don't poll the NDIFinderService for network sources, we subscribe to the change notification.
	// Now we need to update the Network Source Collection, but we need to do it in a thread-safe way.

	FScopeLock Lock(&CollectionSyncContext);

	// Check to determine if something actually changed within the collection. We don't want to trigger
	// notifications unnecessarily.
	if (FNDIFinderService::UpdateSourceCollection(NetworkSourceCollection))
	{
		// Trigger the blueprint handling of the situation.
		this->OnNetworkSourcesChangedEvent();

		// If any listeners have subscribed broadcast any collection changes
		if (this->OnNetworkSourcesChanged.IsBound())
			this->OnNetworkSourcesChanged.Broadcast(this);
	}
}

/**
	Attempts to find a network source by the supplied name.

	@param ConnectionInformation An existing source information structure which contains the source name
	@param InSourceName A string value representing the name of the source to find
	@result A value indicating whether a source with the supplied name was found
*/
const bool UNDIFinderComponent::FindNetworkSourceByName(FNDIConnectionInformation& ConnectionInformation, FString InSourceName)
{
	// Lock the Collection so that we are working with a solid collection of items
	FScopeLock Lock(&CollectionSyncContext);

	// Ensure we Reset the SourceInformation
	ConnectionInformation.Reset();

	// Get the current number of network source items in the collection
	int32 final_count = this->NetworkSourceCollection.Num();

	// Ensure we have a wide range of items to search through.
	int32 last_index = final_count;

	// Since the Source collection returned is already sorted alphabetically do a binary search to speed things up.
	// We are only going to do comparisons that are necessary using O(log(n)) time complexity
	for (int32 current_index = 0; current_index < last_index; current_index)
	{
		// Ensure that the index is valid (this will include negative values
		if (NetworkSourceCollection.IsValidIndex(current_index))
		{
			// Get the source reference from the collection
			FNDIConnectionInformation& source_info = NetworkSourceCollection[current_index];

			// do a comparison against the requested SourceName
			if (int32 comparitor_value = InSourceName.Compare(source_info.SourceName, ESearchCase::IgnoreCase))
			{
				// Our search says that our source name is greater than the info we checked
				if (comparitor_value <= 0)
				{
					// set the last index to the current index
					last_index = current_index;

					// get halfway between the last index and the 0th index
					current_index = last_index / 2;
				}

				// Our search says that our source name is less than the info we checked
				else if (comparitor_value > 0)
				{
					// move up half the number of items within the collection
					current_index = (last_index + current_index + 1) / 2;
				}
			}

			// We found a comparable source.
			else
			{
				// Set the source information structure 
				ConnectionInformation = source_info;

				// return success
				return true;
			}
		}

		// Something weird happened (maybe the first check was larger than the search term); just return a fail
		else return false;
	}

	return false;
}

/**
	Returns the current collection of sources found on the network
*/
const TArray<FNDIConnectionInformation> UNDIFinderComponent::GetNetworkSources()
{
	// Lock the current source collection
	FScopeLock Lock(&CollectionSyncContext);

	// return the source collection
	return this->NetworkSourceCollection;
}