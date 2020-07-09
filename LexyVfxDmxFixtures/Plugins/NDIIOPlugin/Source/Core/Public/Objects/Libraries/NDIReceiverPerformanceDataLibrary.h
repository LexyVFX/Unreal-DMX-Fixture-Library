/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#pragma once

#include <CoreMinimal.h>
#include <Kismet/BlueprintFunctionLibrary.h>
#include <Structures/NDIReceiverPerformanceData.h>

#include "NDIReceiverPerformanceDataLibrary.generated.h"

UCLASS(NotBlueprintable, BlueprintType, Category = "NDI IO", META = (DisplayName = "NDI Reciever Performance Data Library"))
class NDIIO_API UNDIReceiverPerformanceDataLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	private:
		/** 
			Returns a value indicating whether the two structures are comparably equal

			@param A The structure used as the source comparator
			@param B The structure used as the target comparator
			@return The resulting value of the comparator operator
		*/
		UFUNCTION(BlueprintCallable, BlueprintPure, Category = "NDI IO", META = (DisplayName = "Equals (NDI Receiver Performance Data)", CompactNodeTitle = "==", Keywords = "= == Equals", AllowPrivateAccess = true))
		static bool K2_Compare_NDIReceiverPerformanceData(FNDIReceiverPerformanceData A, FNDIReceiverPerformanceData B) { return A == B; }

		/** 
			Returns a value indicating whether the two structures are NOT comparably equal

			@param A The structure used as the source comparator
			@param B The structure used as the target comparator
			@return The resulting value of the comparator operator
		*/
		UFUNCTION(BlueprintCallable, BlueprintPure, Category = "NDI IO", META = (DisplayName = "Not Equals (NDI Receiver Performance Data)", CompactNodeTitle = "!=", Keywords = "! != Not Equals", AllowPrivateAccess = true))
		static bool K2_Compare_Not_NDIReceiverPerformanceData(FNDIReceiverPerformanceData A, FNDIReceiverPerformanceData B) { return A != B; }

		/**
			Resets the structure's properties to their default values

			@param PerformanceData The structure to reset to the default value
			@return The reference to the passed in structure after the 'reset' has been completed
		*/
		UFUNCTION(BlueprintCallable, Category = "NDI IO", META = (DisplayName = "Reset Receiver Performance Data", AllowPrivateAccess = true))
		static UPARAM(ref)FNDIReceiverPerformanceData& K2_NDIReceiverPerformanceData_Reset(UPARAM(ref)FNDIReceiverPerformanceData& PerformanceData)
		{
			// call the underlying function to reset the properties of the object
			PerformanceData.Reset();

			// return the Performance Data object reference
			return PerformanceData;
		}
};