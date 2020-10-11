// Fill out your copyright notice in the Description page of Project Settings.


#include "LexyVFXDMXDimmerComponent.h"

void ULexyVFXDMXDimmerComponent::BeginPlay()
{
	Super::BeginPlay();
	this->SetParentDMXRef();
	this->InitDMXFunctionNames(TArray<FName>({ "dimmer" }));

	SpotRef_Light = Cast<USpotLightComponent>(this->FindComponentsByName(USpotLightComponent::StaticClass(), TArray<FString>({ "spot" }))[0]);

	SMRef_Beam = Cast<UStaticMeshComponent>(this->FindComponentsByName(UStaticMeshComponent::StaticClass(), TArray<FString>({ "beam" }))[0]);

	SMRef_Lens = Cast<UStaticMeshComponent>(this->FindComponentsByName(UStaticMeshComponent::StaticClass(), TArray<FString>({ "lens" }))[0]);

	miBeam = SMRef_Beam->CreateDynamicMaterialInstance(0, SMRef_Beam->GetMaterial(0));

	miLens = SMRef_Lens->CreateDynamicMaterialInstance(0, SMRef_Lens->GetMaterial(0));
}

void ULexyVFXDMXDimmerComponent::UpdateDMX(TMap<FName, int32> NImapDMXFunctionValues, TArray<FName> nDMXComponentFunctions)
{
	this->UpdateDMXMaterialScalarParameter(miBeam, dimmerBitDepth, FName("DMX Dimmer"), 1.0f, 0.0f, 1.0f, NImapDMXFunctionValues, this->FunctionNames.nDMXComponentFunctions[0]);

	this->UpdateDMXMaterialScalarParameter(miLens, dimmerBitDepth, FName("DMX Dimmer"), 1.0f, 0.0f, 1.0f, NImapDMXFunctionValues, this->FunctionNames.nDMXComponentFunctions[0]);

	this->UpdateDMXLightIntensity(dimmerBitDepth, SpotRef_Light, fLightIntensity, NImapDMXFunctionValues, this->FunctionNames.nDMXComponentFunctions[0]);
}