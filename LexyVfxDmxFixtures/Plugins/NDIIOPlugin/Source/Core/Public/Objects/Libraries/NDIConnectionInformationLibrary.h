/*
	All rights reserved. Copyright(c) 2018-2020, NewTek Inc.

	This file and it's use within a Product is bound by the terms of NDI SDK license that was provided
	as part of the NDI SDK. For more information, please review the license and the NDI SDK documentation.
*/

#pragma once

#include <CoreMinimal.h>
#include <Kismet/BlueprintFunctionLibrary.h>
#include <Structures/NDIConnectionInformation.h>

#include "NDIConnectionInformationLibrary.generated.h"

UCLASS(NotBlueprintable, BlueprintType, Category = "NDI IO", META = (DisplayName = "NDI Connection Information Library"))
class NDIIO_API UNDIConnectionInformationLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	private:
		/** 
			Returns a value indicating whether the two structures are comparably equal

			@param A The structure used as the source comparator
			@param B The structure used as the target comparator
			@return The resulting value of the comparator operator
		*/
		UFUNCTION(BlueprintCallable, BlueprintPure, Category = "NDI IO", META = (DisplayName = "Equals (NDI Connection Information)", CompactNodeTitle = "==", Keywords = "= == Equals", AllowPrivateAccess = true))
		static bool K2_Compare_NDIConnectionInformation(FNDIConnectionInformation A, FNDIConnectionInformation B) { return A == B; }

		/** 
			Returns a value indicating whether the two structures are NOT comparably equal

			@param A The structure used as the source comparator
			@param B The structure used as the target comparator
			@return The resulting value of the comparator operator
		*/
		UFUNCTION(BlueprintCallable, BlueprintPure, Category = "NDI IO", META = (DisplayName = "Not Equals (NDI Connection Information)", CompactNodeTitle = "!=", Keywords = "! != Not Equals", AllowPrivateAccess = true))
		static bool K2_Compare_Not_NDIConnectionInformation(FNDIConnectionInformation A, FNDIConnectionInformation B) { return A != B; }

		/** 
			Returns a value indicating whether the property values of the supplied structure is valid

			@param ConnectionInformation The structure to validate
			@return An indication of the supplied structures validity
		*/
		UFUNCTION(BlueprintCallable, Category = "NDI IO", META = (DisplayName = "Is Valid?", AllowPrivateAccess = true))
		static bool K2_NDIConnectionInformation_IsValid(FNDIConnectionInformation& ConnectionInformation) { return ConnectionInformation.IsValid(); }
		
		/**
			Resets the structure's properties to their default values

			@param ConnectionInformation The structure to reset to the default value
			@return The reference to the passed in structure after the 'reset' has been completed
		*/
		UFUNCTION(BlueprintCallable, Category = "NDI IO", META = (DisplayName = "Reset Connection Information", AllowPrivateAccess = true))
		static UPARAM(ref)FNDIConnectionInformation& K2_NDIConnectionInformation_Reset(UPARAM(ref)FNDIConnectionInformation& ConnectionInformation)
		{
			// call the underlying function to reset the properties of the object
			ConnectionInformation.Reset();

			// return the ConnectionInformation object reference
			return ConnectionInformation;
		}
};