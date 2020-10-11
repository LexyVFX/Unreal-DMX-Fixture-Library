// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LexyVFXDMXBaseComponent.h"
#include "LexyVFXDMXZoomComponent.generated.h"

/**
 * 
 */
UCLASS( ClassGroup = (DMXFunctions), meta = (BlueprintSpawnableComponent) )
class LEXYVFXCPPFIXTURES_API ULexyVFXDMXZoomComponent : public ULexyVFXDMXBaseComponent
{
	GENERATED_BODY()
	
protected:
	void BeginPlay() override;
public:
	void UpdateDMX(TMap<FName, int32> NImapDMXFunctionValues, TArray<FName> nDMXComponentFunctions) override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent *SMRef_Beam;

	UPROPERTY(EditAnywhere)
	USpringArmComponent *SPRef_LensSpringArm;

	UPROPERTY(EditAnywhere)
	USpotLightComponent *SpotRef_Light;

	UPROPERTY(EditAnywhere)
	UMaterialInstanceDynamic *miBeam;

	UPROPERTY(EditAnywhere)
	EDMXParameterBitDepth zoomBitDepth;

	UPROPERTY(EditAnywhere)
	float fBeamRangeLinear = 4.69101f;

	UPROPERTY(EditAnywhere)
	float fBeamRangeMin = 3.7f;

	UPROPERTY(EditAnywhere)
	float fBeamRangeMax = 35.0f;
};
