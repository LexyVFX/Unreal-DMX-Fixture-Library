/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#pragma once

#include <CoreMinimal.h>

#include <HAL/Runnable.h>
#include <HAL/ThreadSafeBool.h>
#include <Structures/NDIConnectionInformation.h>

/** 
	A Runnable object used for Finding NDI network Sources, and updating interested parties
*/
class NDIIO_API FNDIFinderService : public FRunnable
{
	public:
		FNDIFinderService();

		// Begin the service
		virtual bool Start();

		// Stop the service
		virtual void Shutdown();

	public:
		/** Get the available sources on the network */
		static const TArray<FNDIConnectionInformation> GetNetworkSourceCollection();

		/** Call to update an existing collection of network sources to match the current collection */
		static bool UpdateSourceCollection(TArray<FNDIConnectionInformation>& InSourceCollection);

		/** Event which is triggered when the collection of network sources has changed */
		DECLARE_EVENT(FNDICoreDelegates, FNDISourceCollectionChangedEvent)
		static FNDISourceCollectionChangedEvent EventOnNDISourceCollectionChanged;

	protected:		
		/** FRunnable Interface implementation for 'Init' */
		virtual bool Init() override;

		/** FRunnable Interface implementation for 'Stop' */
		virtual void Stop() override;

		/** FRunnable Interface implementation for 'Run' */
		virtual uint32 Run() override;

	private:
		bool UpdateNetworkSourceCollection();

	private:
		bool bShouldWaitOneFrame = true;
		bool bIsNetworkSourceCollectionDirty = false;

		FThreadSafeBool bIsThreadRunning;
		FRunnableThread* p_RunnableThread = nullptr;

		static TArray<FNDIConnectionInformation> NetworkSourceCollection;
};