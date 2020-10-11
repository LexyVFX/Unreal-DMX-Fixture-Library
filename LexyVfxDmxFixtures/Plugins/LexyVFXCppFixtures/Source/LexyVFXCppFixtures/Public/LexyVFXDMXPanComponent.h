// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LexyVFXDMXBaseComponent.h"
#include "LexyVFXDMXPanComponent.generated.h"

/**
 * 
 */
UCLASS( ClassGroup = (DMXFunctions), meta = (BlueprintSpawnableComponent) )
class LEXYVFXCPPFIXTURES_API ULexyVFXDMXPanComponent : public ULexyVFXDMXBaseComponent
{
	GENERATED_BODY()
	
protected:
	void BeginPlay() override;
public:
	void UpdateDMX(TMap<FName, int32> NImapDMXFunctionValues, TArray<FName> nDMXComponentFunctions) override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent *SMRef_Yoke;

	UPROPERTY(EditAnywhere)
	EDMXParameterBitDepth panBitDepth;

	UPROPERTY(EditAnywhere)
	float fPanRange = 540.0f;
};
