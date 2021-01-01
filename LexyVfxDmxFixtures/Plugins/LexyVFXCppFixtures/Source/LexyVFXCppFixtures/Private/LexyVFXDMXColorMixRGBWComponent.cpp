// Fill out your copyright notice in the Description page of Project Settings.


#include "LexyVFXDMXColorMixRGBWComponent.h"

void ULexyVFXDMXColorMixRGBWComponent::BeginPlay()
{
	Super::BeginPlay();
	this->SetParentDMXRef();
	this->InitDMXFunctionNames(TArray<FName>({ "Red", "Green", "Blue", "White" }));

	SpotRef_Light = Cast<USpotLightComponent>(this->FindComponentsByName(USpotLightComponent::StaticClass(), TArray<FString>({ "spot" }))[0]);

	SMRef_Beam = Cast<UStaticMeshComponent>(this->FindComponentsByName(UStaticMeshComponent::StaticClass(), TArray<FString>({ "beam" }))[0]);

	SMRef_Lens = Cast<UStaticMeshComponent>(this->FindComponentsByName(UStaticMeshComponent::StaticClass(), TArray<FString>({ "lens" }))[0]);

	miBeam = SMRef_Beam->CreateDynamicMaterialInstance(0, SMRef_Beam->GetMaterial(0));

	miLens = SMRef_Lens->CreateDynamicMaterialInstance(0, SMRef_Lens->GetMaterial(0));
}

void ULexyVFXDMXColorMixRGBWComponent::UpdateDMX(TMap<FDMXAttributeName, int32> DImapDMXFunctionValues, TArray<FName> nDMXComponentFunctions)
{
	this->UpdateDMXMaterialVectorParameter(miBeam, colorMixRGBWBitDepth, FName("DMX Color"), DImapDMXFunctionValues, this->FunctionNames.nDMXComponentFunctions);

	this->UpdateDMXMaterialVectorParameter(miLens, colorMixRGBWBitDepth, FName("DMX Color"), DImapDMXFunctionValues, this->FunctionNames.nDMXComponentFunctions);

	this->UpdateDMXLightColor(colorMixRGBWBitDepth, SpotRef_Light, 1.0f, DImapDMXFunctionValues, this->FunctionNames.nDMXComponentFunctions);
}