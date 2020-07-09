/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#include <Objects/Libraries/NDIIOLibrary.h>
#include <Services/NDIFinderService.h>
#include <NDIIOPluginModule.h>

const TArray<FNDIConnectionInformation> UNDIIOLibrary::K2_GetNDISourceCollection()
{
	// Return the FinderServices current network source collection
	return FNDIFinderService::GetNetworkSourceCollection();
}

const bool UNDIIOLibrary::K2_FindNetworkSourceByName(UObject* WorldContextObject, FNDIConnectionInformation& ConnectionInformation, FString InSourceName)
{
	// Ensure that the passed in information is empty
	ConnectionInformation.Reset();

	// Get the current network source collection from the finder service.
	const TArray<FNDIConnectionInformation> NetworkSourceCollection = FNDIFinderService::GetNetworkSourceCollection();	
	
	// Get the current number of network source items in the collection
	int32 final_count = NetworkSourceCollection.Num();

	// Ensure we have a wide range of items to search through.
	int32 last_index = final_count;

	// Since the Source collection returned is already sorted alphabetically do a binary search to speed things up.
	// We are only going to do comparisons that are necessary using O(log(n)) time complexity
	for (int32 current_index = 0; current_index < last_index; current_index)
	{
		// Ensure that the index is valid (this will protect against negative values)
		if (NetworkSourceCollection.IsValidIndex(current_index))
		{
			// Get the source reference from the collection
			FNDIConnectionInformation source_info = NetworkSourceCollection[current_index];

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

bool UNDIIOLibrary::K2_BeginBroadcastingActiveViewport(UObject* WorldContextObject)
{
	// Get the plugin module for the owner of this object
	if (FNDIIOPluginModule* PluginModule = FModuleManager::GetModulePtr<FNDIIOPluginModule>("NDIIO"))
	{
		// Call the underlying functionality
		return PluginModule->BeginBroadcastingActiveViewport();
	}

	return false;
}

void UNDIIOLibrary::K2_StopBroadcastingActiveViewport(UObject* WorldContextObject)
{
	// Get the plugin module for the owner of this object
	if (FNDIIOPluginModule* PluginModule = FModuleManager::GetModulePtr<FNDIIOPluginModule>("NDIIO"))
	{
		// Call the underlying functionality
		PluginModule->StopBroadcastingActiveViewport();
	}
}

UNDIMediaReceiver* UNDIIOLibrary::K2_GetNDIMediaReceiver(UNDIMediaReceiver* Receiver) { return Receiver; }

UNDIMediaSender* UNDIIOLibrary::K2_GetNDIMediaSender(UNDIMediaSender* Sender) { return Sender; }