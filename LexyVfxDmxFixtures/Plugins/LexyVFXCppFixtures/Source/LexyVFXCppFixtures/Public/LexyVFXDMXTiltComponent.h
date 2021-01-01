// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LexyVFXDMXBaseComponent.h"
#include "LexyVFXDMXTiltComponent.generated.h"

/**
 * 
 */
UCLASS( ClassGroup = (DMXFunctions), meta = (BlueprintSpawnableComponent) )
class LEXYVFXCPPFIXTURES_API ULexyVFXDMXTiltComponent : public ULexyVFXDMXBaseComponent
{
	GENERATED_BODY()
	
protected:
	void BeginPlay() override;
public:
	void UpdateDMX(TMap<FDMXAttributeName, int32> DImapDMXFunctionValues, TArray<FName> nDMXComponentFunctions) override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent *SMRef_Head;

	UPROPERTY(EditAnywhere)
	EDMXParameterBitDepth tiltBitDepth;

	UPROPERTY(EditAnywhere)
	float fTiltRange = 250.0f;
};
