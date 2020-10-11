// Fill out your copyright notice in the Description page of Project Settings.


#include "LexyVFXDMXBaseComponent.h"

// Sets default values for this component's properties
ULexyVFXDMXBaseComponent::ULexyVFXDMXBaseComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULexyVFXDMXBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void ULexyVFXDMXBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void ULexyVFXDMXBaseComponent::SetParentDMXRef()
{
}

void ULexyVFXDMXBaseComponent::InitDMXFunctionNames(TArray<FName> DMXFunctionNames)
{
}

TArray<UActorComponent*> ULexyVFXDMXBaseComponent::FindComponentsByName(TSubclassOf<UActorComponent> ComponentType, TArray<FString> searchNames)
{
	return TArray<UActorComponent*>();
}

void ULexyVFXDMXBaseComponent::UpdateDMX(TMap<FName, int32> NImapDMXFunctionValues, TArray<FName> nDMXComponentFunctions)
{
}

void ULexyVFXDMXBaseComponent::UpdateDMXMaterialScalarParameter(UMaterialInstanceDynamic * miTargetMaterial, EDMXParameterBitDepth DMXBitDepth, FName nMaterialParameterName, float fScaleFactor, float fRangeMin, float fRangeMax, TMap<FName, int32> NImapDMXFunctionValues, FName nDMXComponentFunction)
{
}

void ULexyVFXDMXBaseComponent::UpdateDMXMaterialVectorParameter(UMaterialInstanceDynamic * miTargetMaterial, EDMXParameterBitDepth DMXBitDepth, FName nMaterialParameterName, TMap<FName, int32> NImapDMXFunctionValues, TArray<FName> nDMXComponentFunctions)
{
}

void ULexyVFXDMXBaseComponent::UpdateDMXLightColor(EDMXParameterBitDepth DMXBitDepth, ULightComponent * LightComponentRef, float fRange, TMap<FName, int32> NImapDMXFunctionValues, TArray<FName> nDMXComponentFunctions)
{
}

void ULexyVFXDMXBaseComponent::UpdateDMXSpringArm(EDMXParameterBitDepth DMXBitDepth, USpringArmComponent * SpringArmComponentRef, float fRange, TMap<FName, int32> NImapDMXFunctionValues, FName nDMXComponentFunction)
{
}

bool ULexyVFXDMXBaseComponent::UpdateDMXSpotConeAngle(EDMXParameterBitDepth DMXBitDepth, ULightComponent * LightComponentRef, float fBeamRangeMax, float fBeamRangeMin, TMap<FName, int32> NImapDMXFunctionValues, FName nDMXComponentFunction)
{
	return false;
}

void ULexyVFXDMXBaseComponent::UpdateDMXLightIntensity(EDMXParameterBitDepth DMXBitDepth, ULightComponent * LightComponentRef, float fRange, TMap<FName, int32> NImapDMXFunctionValues, FName nDMXComponentFunction)
{
}

bool ULexyVFXDMXBaseComponent::UpdateDMXRotation(EDMXParameterBitDepth DMXBitDepth, USceneComponent * SceneComponentRef, EDMXRotationMode eRotationMode, float fRange, TMap<FName, int32> NImapDMXFunctionValues, FName nDMXComponentFunction)
{
	return false;
}

