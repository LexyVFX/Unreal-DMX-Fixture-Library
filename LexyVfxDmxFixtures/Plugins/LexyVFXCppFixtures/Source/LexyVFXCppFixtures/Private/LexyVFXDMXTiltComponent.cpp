// Fill out your copyright notice in the Description page of Project Settings.


#include "LexyVFXDMXTiltComponent.h"

void ULexyVFXDMXTiltComponent::BeginPlay()
{
	Super::BeginPlay();
	this->SetParentDMXRef();
	this->InitDMXFunctionNames(TArray<FName>({ "Tilt" }));

	SMRef_Head = Cast<UStaticMeshComponent>(this->FindComponentsByName(UStaticMeshComponent::StaticClass(), TArray<FString>({ "head" }))[0]);
}

void ULexyVFXDMXTiltComponent::UpdateDMX(TMap<FDMXAttributeName, int32> DImapDMXFunctionValues, TArray<FName> nDMXComponentFunctions)
{
	this->UpdateDMXRotation(tiltBitDepth, SMRef_Head, EDMXRotationMode::RotationMode_Tilt, fTiltRange, DImapDMXFunctionValues, this->FunctionNames.nDMXComponentFunctions[0]);
}