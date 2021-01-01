// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LexyVFXDMXBaseComponent.h"
#include "LexyVFXDMXDimmerComponent.generated.h"

/**
 * 
 */
UCLASS( ClassGroup = (DMXFunctions), meta = (BlueprintSpawnableComponent) )
class LEXYVFXCPPFIXTURES_API ULexyVFXDMXDimmerComponent : public ULexyVFXDMXBaseComponent
{
	GENERATED_BODY()
	
protected:
	void BeginPlay() override;
public:
	void UpdateDMX(TMap<FDMXAttributeName, int32> DImapDMXFunctionValues, TArray<FName> nDMXComponentFunctions) override;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent *SMRef_Beam;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent *SMRef_Lens;

	UPROPERTY(EditAnywhere)
	USpotLightComponent *SpotRef_Light;

	UPROPERTY(EditAnywhere)
	UMaterialInstanceDynamic *miBeam;

	UPROPERTY(EditAnywhere)
	UMaterialInstanceDynamic *miLens;

	UPROPERTY(EditAnywhere)
	EDMXParameterBitDepth dimmerBitDepth;

	UPROPERTY(EditAnywhere)
	float fLightIntensity = 60000.0f;
};
