/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#pragma once

#include <CoreMinimal.h>
#include <Kismet/BlueprintFunctionLibrary.h>
#include <Structures/NDIBroadcastConfiguration.h>

#include "NDIBroadcastConfigurationLibrary.generated.h"

UCLASS(NotBlueprintable, BlueprintType, Category = "NDI IO", META = (DisplayName = "NDI Broadcast Configuration Library"))
class NDIIO_API UNDIBroadcastConfigurationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	private:
		/** 
			Returns a value indicating whether the two structures are comparably equal

			@param A The structure used as the source comparator
			@param B The structure used as the target comparator
			@return The resulting value of the comparator operator
		*/
		UFUNCTION(BlueprintCallable, BlueprintPure, Category = "NDI IO", META = (DisplayName = "Equals (NDI Broadcast Configuration)", CompactNodeTitle = "==", Keywords = "= == Equals", AllowPrivateAccess = true))
		static bool K2_Compare_NDIBroadcastConfiguration(FNDIBroadcastConfiguration A, FNDIBroadcastConfiguration B) { return A == B; }

		/** 
			Returns a value indicating whether the two structures are NOT comparably equal

			@param A The structure used as the source comparator
			@param B The structure used as the target comparator
			@return The resulting value of the comparator operator
		*/
		UFUNCTION(BlueprintCallable, BlueprintPure, Category = "NDI IO", META = (DisplayName = "Not Equals (NDI Broadcast Configuration)", CompactNodeTitle = "!=", Keywords = "! != Not Equals", AllowPrivateAccess = true))
		static bool K2_Compare_Not_NDIBroadcastConfiguration(FNDIBroadcastConfiguration A, FNDIBroadcastConfiguration B) { return A != B; }
};