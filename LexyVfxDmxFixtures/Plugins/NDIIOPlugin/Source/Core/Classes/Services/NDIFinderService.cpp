/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#include <Services/NDIFinderService.h>
#include <Async/Async.h>
#include <NDIIOPluginAPI.h>	

/** Define Global Accessors */

static void* NDI_FIND_INSTANCE = nullptr;
static FCriticalSection NDI_FIND_SYNC_CONTEXT;

FNDIFinderService::FNDISourceCollectionChangedEvent FNDIFinderService::EventOnNDISourceCollectionChanged;

TArray<FNDIConnectionInformation> FNDIFinderService::NetworkSourceCollection = TArray<FNDIConnectionInformation>();

/** ************************ **/

FNDIFinderService::FNDIFinderService()
{
	if (NDI_FIND_INSTANCE == nullptr)
	{
		FScopeLock Lock(&NDI_FIND_SYNC_CONTEXT);		

		NDI_FIND_INSTANCE = NDIlib_find_create_v2(nullptr);
	}
}

// Begin the service
bool FNDIFinderService::Start()
{
	if (!bIsThreadRunning && p_RunnableThread == nullptr)
	{
		if (NDI_FIND_INSTANCE != nullptr)
		{
			this->bIsThreadRunning = true;
			p_RunnableThread = FRunnableThread::Create(this, TEXT("FNDIFinderService_Tick"), 0, TPri_BelowNormal);

			return bIsThreadRunning = p_RunnableThread != nullptr;
		}
	}

	return false;
}

/** FRunnable Interface implementation for 'Init' */
bool FNDIFinderService::Init() { return NDI_FIND_INSTANCE != nullptr; }

/** FRunnable Interface implementation for 'Stop' */
uint32 FNDIFinderService::Run()
{
	static const uint32 find_wait_time = 500;

	if (NDI_FIND_INSTANCE == nullptr)
		return 0;	

	// Only update when we are suppose to run
	while (bIsThreadRunning)
	{
		// Wait up to 'find_wait_time' (in milliseconds) to determine whether new sources have been added
		if (!NDIlib_find_wait_for_sources(NDI_FIND_INSTANCE, find_wait_time))
		{
			// alright the source collection has stopped updating, did we change the network source collection?
			if (UpdateNetworkSourceCollection())
			{
				// Broadcast the even on the game thread for thread safety purposes
				AsyncTask(ENamedThreads::GameThread, []() {

					if (FNDIFinderService::EventOnNDISourceCollectionChanged.IsBound())
						FNDIFinderService::EventOnNDISourceCollectionChanged.Broadcast();
				});
			}
		}
	}

	// return success
	return 1;
}

/** FRunnable Interface implementation for 'Run' */
void FNDIFinderService::Shutdown()
{
	if (p_RunnableThread != nullptr)
	{
		this->bIsThreadRunning = false;

		p_RunnableThread->WaitForCompletion();
		p_RunnableThread = nullptr;
	}

	// Ensure we unload the finder instance
	if (NDI_FIND_INSTANCE != nullptr)
		NDIlib_find_destroy(NDI_FIND_INSTANCE);
}

// Stop the service
void FNDIFinderService::Stop() { Shutdown(); }

bool FNDIFinderService::UpdateNetworkSourceCollection()
{
	uint32 no_sources = 0;
	bool bHasCollectionChanged = false;

	if (NDI_FIND_INSTANCE != nullptr)
	{
		const NDIlib_source_t* p_sources = NDIlib_find_get_current_sources(NDI_FIND_INSTANCE, &no_sources);

		// Change Scope
		{
			FScopeLock lock(&NDI_FIND_SYNC_CONTEXT);

			bHasCollectionChanged = FNDIFinderService::NetworkSourceCollection.Num() != no_sources;

			if (no_sources > 0 && p_sources != nullptr)
			{
				uint32 CurrentSourceCount = NetworkSourceCollection.Num();

				for (uint32 iter = 0; iter < no_sources; iter++)
				{
					if (iter >= CurrentSourceCount)
					{
						NetworkSourceCollection.Add(FNDIConnectionInformation());
					}	

					const NDIlib_source_t* SourceInformation = &p_sources[iter];
					FNDIConnectionInformation* CollectionSource = &NetworkSourceCollection[iter];

					bHasCollectionChanged |= SourceInformation->p_url_address != CollectionSource->Url;
					CollectionSource->Url = SourceInformation->p_url_address;
					CollectionSource->SourceName = SourceInformation->p_ndi_name;
					CollectionSource->SourceName.Split(TEXT(" "), &CollectionSource->MachineName, &CollectionSource->StreamName);

					// Now that the MachineName and StreamName have been split, cleanup the stream name
					CollectionSource->StreamName.RemoveFromStart("(");
					CollectionSource->StreamName.RemoveFromEnd(")");
				}

				if (CurrentSourceCount > no_sources)
				{
					NetworkSourceCollection.RemoveAt(no_sources, CurrentSourceCount - no_sources, true);
					bHasCollectionChanged = true;
				}

			}
			else if (NetworkSourceCollection.Num() > 0)
			{
				NetworkSourceCollection.Empty();
				bHasCollectionChanged = true;
			}

			bHasCollectionChanged |= NetworkSourceCollection.Num() != no_sources;
		}
	}	

	return bHasCollectionChanged;
}

/** Call to update an existing collection of network sources to match the current collection */
bool FNDIFinderService::UpdateSourceCollection(TArray<FNDIConnectionInformation>& InSourceCollection)
{
	bool bHasCollectionChanged = false;

	{
		FScopeLock Lock(&NDI_FIND_SYNC_CONTEXT);

		const uint32& no_sources = NetworkSourceCollection.Num();
		bHasCollectionChanged = InSourceCollection.Num() != no_sources;

		if (no_sources > 0)
		{
			uint32 CurrentSourceCount = InSourceCollection.Num();

			for (uint32 iter = 0; iter < no_sources; iter++)
			{
				if (iter >= CurrentSourceCount)
				{
					InSourceCollection.Add(FNDIConnectionInformation());
					CurrentSourceCount = InSourceCollection.Num();
				}

				FNDIConnectionInformation* CollectionSource = &InSourceCollection[iter];
				const FNDIConnectionInformation* SourceInformation = &NetworkSourceCollection[iter];

				bHasCollectionChanged |= SourceInformation->Url != CollectionSource->Url;

				CollectionSource->Url = SourceInformation->Url;
				CollectionSource->SourceName = SourceInformation->SourceName;
				CollectionSource->MachineName = SourceInformation->MachineName;
				CollectionSource->StreamName = SourceInformation->StreamName;
			}

			if (CurrentSourceCount > no_sources)
			{
				InSourceCollection.RemoveAt(no_sources, CurrentSourceCount - no_sources, true);
				bHasCollectionChanged = true;
			}
		}
		else if (InSourceCollection.Num() > 0)
		{
			InSourceCollection.Empty();
			bHasCollectionChanged = true;
		}
	}

	return bHasCollectionChanged;
}

/** Get the available sources on the network */
const TArray<FNDIConnectionInformation> FNDIFinderService::GetNetworkSourceCollection()
{
	FScopeLock Lock(&NDI_FIND_SYNC_CONTEXT);

	return FNDIFinderService::NetworkSourceCollection;
}
