// Fill out your copyright notice in the Description page of Project Settings.


#include "LexyVFXDMXPanComponent.h"

void ULexyVFXDMXPanComponent::BeginPlay()
{
	Super::BeginPlay();
	this->SetParentDMXRef();
	this->InitDMXFunctionNames(TArray<FName>({ "pan" }));

	SMRef_Yoke = Cast<UStaticMeshComponent>(this->FindComponentsByName(UStaticMeshComponent::StaticClass(), TArray<FString>({ "yoke" }))[0]);
}

void ULexyVFXDMXPanComponent::UpdateDMX(TMap<FName, int32> NImapDMXFunctionValues, TArray<FName> nDMXComponentFunctions)
{
	this->UpdateDMXRotation(panBitDepth, SMRef_Yoke, EDMXRotationMode::RotationMode_Pan, fPanRange, NImapDMXFunctionValues, this->FunctionNames.nDMXComponentFunctions[0]);
}