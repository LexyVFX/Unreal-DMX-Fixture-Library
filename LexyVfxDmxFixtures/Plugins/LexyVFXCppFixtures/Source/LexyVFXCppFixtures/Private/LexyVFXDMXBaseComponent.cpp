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
	DMXComp = Cast<UDMXComponent>(this->GetOwner()->GetComponentByClass(UDMXComponent::StaticClass()));
	if (DMXComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found DMX Component: %s"), *this->GetOwner()->GetComponentByClass(UDMXComponent::StaticClass())->GetName());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't find valid DMX Component on parent actor"));
	}

	Patch = Cast<UDMXEntity>(DMXComp->GetFixturePatch());
	if (Patch)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found Fixture Patch on DMX Component: %s"), *Patch->Name);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't find valid DMX Patch on DMX Component"));
	}
}

void ULexyVFXDMXBaseComponent::InitDMXFunctionNames(TArray<FName> DMXFunctionNames)
{
	this->FunctionNames.nDMXComponentFunctions = DMXFunctionNames;

	for (auto& name : DMXFunctionNames)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s"), *name.ToString());
	}
}

TArray<UActorComponent*> ULexyVFXDMXBaseComponent::FindComponentsByName(TSubclassOf<UActorComponent> ComponentType, TArray<FString> searchNames)
{
	TArray<UActorComponent*> actorComponents;
	this->GetOwner()->GetComponents(ComponentType, actorComponents);

	//TInlineComponentArray<UActorComponent*> actorComponents;
	//this->GetOwner()->GetComponents(actorComponents, false);

	TArray<UActorComponent*> outComps;
	FString currentString;
	FString leftString;
	FString rightString;
	for (int32 i = 0; i != actorComponents.Num(); i++)
	{

		UActorComponent *currentComponent = actorComponents[i];
		UE_LOG(LogTemp, Warning, TEXT("%s"), *currentComponent->GetReadableName());
		UE_LOG(LogTemp, Warning, TEXT("=========================================================================================="));
		//outComps.Add(currentComponent);

		for (int32 j = 0; j != searchNames.Num(); j++)
		{
			currentString = currentComponent->GetReadableName();
			currentString.Split(" ", &leftString, &rightString, ESearchCase::IgnoreCase, ESearchDir::FromStart);

			if (ComponentType == UStaticMeshComponent::StaticClass())
			{
				//UE_LOG(LogTemp, Warning, TEXT("ComponentType is of type UStaticMeshComponent::StaticClass()"));
				if (rightString.Contains(searchNames[j], ESearchCase::IgnoreCase))
				{
					outComps.Add(currentComponent);
					UE_LOG(LogTemp, Warning, TEXT("%s"), *rightString);
					UE_LOG(LogTemp, Warning, TEXT("=========================================================================================="));
				}
			}
			else if (currentString.Contains(searchNames[j], ESearchCase::IgnoreCase))
			{
				outComps.Add(currentComponent);
				UE_LOG(LogTemp, Warning, TEXT("%s"), *currentString);
				UE_LOG(LogTemp, Warning, TEXT("=========================================================================================="));
			}

		}

	}
	return outComps;
}

void ULexyVFXDMXBaseComponent::UpdateDMX(TMap<FDMXAttributeName, int32> DImapDMXFunctionValues, TArray<FName> nDMXComponentFunctions)
{
}

void ULexyVFXDMXBaseComponent::UpdateDMXMaterialScalarParameter(UMaterialInstanceDynamic * miTargetMaterial, EDMXParameterBitDepth DMXBitDepth, FName nMaterialParameterName, float fScaleFactor, float fRangeMin, float fRangeMax, TMap<FDMXAttributeName, int32> DImapDMXFunctionValues, FName nDMXComponentFunction)
{
	float fMaxParameterRange;
	float fScalar;

	switch (DMXBitDepth)
	{
	case EDMXParameterBitDepth::BitDepth_8bits:
		fMaxParameterRange = 255.0f;
		break;
	case EDMXParameterBitDepth::BitDepth_16bits:
		fMaxParameterRange = 65535.0f;
		break;
	case EDMXParameterBitDepth::BitDepth_24bits:
		fMaxParameterRange = 16777215.0f;
		break;
	default:
		fMaxParameterRange = 255.0f;
		break;
	}

	fScalar = fScaleFactor * FMath::GetMappedRangeValueClamped(FVector2D(0.0f, fMaxParameterRange), FVector2D(fRangeMin, fRangeMax), (float)DImapDMXFunctionValues.FindRef(nDMXComponentFunction));

	miTargetMaterial->SetScalarParameterValue(nMaterialParameterName, fScalar);
}

void ULexyVFXDMXBaseComponent::UpdateDMXMaterialVectorParameter(UMaterialInstanceDynamic * miTargetMaterial, EDMXParameterBitDepth DMXBitDepth, FName nMaterialParameterName, TMap<FDMXAttributeName, int32> DImapDMXFunctionValues, TArray<FName> nDMXComponentFunctions)
{
	float fMaxParameterRange;
	FVector4 inVector4;
	FVector4 outVector4;

	switch (DMXBitDepth)
	{
	case EDMXParameterBitDepth::BitDepth_8bits:
		fMaxParameterRange = 255.0f;
		break;
	case EDMXParameterBitDepth::BitDepth_16bits:
		fMaxParameterRange = 65535.0f;
		break;
	case EDMXParameterBitDepth::BitDepth_24bits:
		fMaxParameterRange = 16777215.0f;
		break;
	default:
		fMaxParameterRange = 255.0f;
		break;
	}

	inVector4.W = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, fMaxParameterRange), FVector2D(0.0f, 1.0f), (float)DImapDMXFunctionValues.FindRef(nDMXComponentFunctions[0]));
	inVector4.X = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, fMaxParameterRange), FVector2D(0.0f, 1.0f), (float)DImapDMXFunctionValues.FindRef(nDMXComponentFunctions[1]));
	inVector4.Y = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, fMaxParameterRange), FVector2D(0.0f, 1.0f), (float)DImapDMXFunctionValues.FindRef(nDMXComponentFunctions[2]));
	inVector4.Z = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, fMaxParameterRange), FVector2D(0.0f, 1.0f), (float)DImapDMXFunctionValues.FindRef(nDMXComponentFunctions[3]));

	outVector4.W = 1.0f;
	outVector4.X = FMath::Min(inVector4.W + inVector4.Z, 1.0f);
	outVector4.Y = FMath::Min(inVector4.X + inVector4.Z, 1.0f);
	outVector4.Z = FMath::Min(inVector4.Y + inVector4.Z, 1.0f);

	miTargetMaterial->SetVectorParameterValue(nMaterialParameterName, FLinearColor(outVector4));
}

void ULexyVFXDMXBaseComponent::UpdateDMXLightColor(EDMXParameterBitDepth DMXBitDepth, ULightComponent * LightComponentRef, float fRange, TMap<FDMXAttributeName, int32> DImapDMXFunctionValues, TArray<FName> nDMXComponentFunctions)
{
	float fMaxParameterRange;
	FVector4 inVector4;
	FVector4 outVector4;

	switch (DMXBitDepth)
	{
	case EDMXParameterBitDepth::BitDepth_8bits:
		fMaxParameterRange = 255.0f;
		break;
	case EDMXParameterBitDepth::BitDepth_16bits:
		fMaxParameterRange = 65535.0f;
		break;
	case EDMXParameterBitDepth::BitDepth_24bits:
		fMaxParameterRange = 16777215.0f;
		break;
	default:
		fMaxParameterRange = 255.0f;
		break;
	}

	inVector4.W = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, fMaxParameterRange), FVector2D(0.0f, 1.0f), (float)DImapDMXFunctionValues.FindRef(nDMXComponentFunctions[0]));
	inVector4.X = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, fMaxParameterRange), FVector2D(0.0f, 1.0f), (float)DImapDMXFunctionValues.FindRef(nDMXComponentFunctions[1]));
	inVector4.Y = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, fMaxParameterRange), FVector2D(0.0f, 1.0f), (float)DImapDMXFunctionValues.FindRef(nDMXComponentFunctions[2]));
	inVector4.Z = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, fMaxParameterRange), FVector2D(0.0f, 1.0f), (float)DImapDMXFunctionValues.FindRef(nDMXComponentFunctions[3]));

	outVector4.W = 1.0f;
	outVector4.X = FMath::Min(inVector4.W + inVector4.Z, 1.0f);
	outVector4.Y = FMath::Min(inVector4.X + inVector4.Z, 1.0f);
	outVector4.Z = FMath::Min(inVector4.Y + inVector4.Z, 1.0f);

	LightComponentRef->SetLightColor(FLinearColor(outVector4), false);
}

void ULexyVFXDMXBaseComponent::UpdateDMXSpringArm(EDMXParameterBitDepth DMXBitDepth, USpringArmComponent * SpringArmComponentRef, float fRange, TMap<FDMXAttributeName, int32> DImapDMXFunctionValues, FName nDMXComponentFunction)
{
	float fMaxParameterRange;

	switch (DMXBitDepth)
	{
	case EDMXParameterBitDepth::BitDepth_8bits:
		fMaxParameterRange = 255.0f;
		break;
	case EDMXParameterBitDepth::BitDepth_16bits:
		fMaxParameterRange = 65535.0f;
		break;
	case EDMXParameterBitDepth::BitDepth_24bits:
		fMaxParameterRange = 16777215.0f;
		break;
	default:
		fMaxParameterRange = 255.0f;
		break;
	}

	SpringArmComponentRef->TargetArmLength = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, fMaxParameterRange), FVector2D(0.0f, fRange), (float)DImapDMXFunctionValues.FindRef(nDMXComponentFunction));
}

bool ULexyVFXDMXBaseComponent::UpdateDMXSpotConeAngle(EDMXParameterBitDepth DMXBitDepth, ULightComponent * LightComponentRef, float fBeamRangeMax, float fBeamRangeMin, TMap<FDMXAttributeName, int32> DImapDMXFunctionValues, FName nDMXComponentFunction)
{
	USpotLightComponent *SpotComponent = Cast<USpotLightComponent>(LightComponentRef);
	float fMaxParameterRange;
	float outFloat;

	switch (DMXBitDepth)
	{
	case EDMXParameterBitDepth::BitDepth_8bits:
		fMaxParameterRange = 255.0f;
		break;
	case EDMXParameterBitDepth::BitDepth_16bits:
		fMaxParameterRange = 65535.0f;
		break;
	case EDMXParameterBitDepth::BitDepth_24bits:
		fMaxParameterRange = 16777215.0f;
		break;
	default:
		fMaxParameterRange = 255.0f;
		break;
	}

	outFloat = 0.7f * FMath::GetMappedRangeValueClamped(FVector2D(0.0f, fMaxParameterRange), FVector2D(fBeamRangeMax, fBeamRangeMin), (float)DImapDMXFunctionValues.FindRef(nDMXComponentFunction));

	if (SpotComponent)
	{
		SpotComponent->OuterConeAngle = outFloat;
		SpotComponent->InnerConeAngle = 0.7f * outFloat;
		return true;
	}
	else
	{
		return false;
	}
}

void ULexyVFXDMXBaseComponent::UpdateDMXLightIntensity(EDMXParameterBitDepth DMXBitDepth, ULightComponent * LightComponentRef, float fRange, TMap<FDMXAttributeName, int32> DImapDMXFunctionValues, FName nDMXComponentFunction)
{
	float fMaxParameterRange;

	switch (DMXBitDepth)
	{
	case EDMXParameterBitDepth::BitDepth_8bits:
		fMaxParameterRange = 255.0f;
		break;
	case EDMXParameterBitDepth::BitDepth_16bits:
		fMaxParameterRange = 65535.0f;
		break;
	case EDMXParameterBitDepth::BitDepth_24bits:
		fMaxParameterRange = 16777215.0f;
		break;
	default:
		fMaxParameterRange = 255.0f;
		break;
	}

	LightComponentRef->Intensity = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, fMaxParameterRange), FVector2D(0.0f, fRange), (float)DImapDMXFunctionValues.FindRef(nDMXComponentFunction));
}

bool ULexyVFXDMXBaseComponent::UpdateDMXRotation(EDMXParameterBitDepth DMXBitDepth, USceneComponent * SceneComponentRef, EDMXRotationMode eRotationMode, float fRange, TMap<FDMXAttributeName, int32> DImapDMXFunctionValues, FName nDMXComponentFunction)
{
	float fMaxParameterRange;
	float outFloat;

	switch (DMXBitDepth)
	{
	case EDMXParameterBitDepth::BitDepth_8bits:
		fMaxParameterRange = 255.0f;
		break;
	case EDMXParameterBitDepth::BitDepth_16bits:
		fMaxParameterRange = 65535.0f;
		break;
	case EDMXParameterBitDepth::BitDepth_24bits:
		fMaxParameterRange = 16777215.0f;
		break;
	default:
		fMaxParameterRange = 255.0f;
		break;
	}


	outFloat = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, fMaxParameterRange), FVector2D(fRange * -0.5f, fRange * 0.5f), (float)DImapDMXFunctionValues.FindRef(nDMXComponentFunction));

	switch (eRotationMode)
	{
	case EDMXRotationMode::RotationMode_Pan:
		SceneComponentRef->SetRelativeRotation(FRotator(0.0f, outFloat, 0.0f).Quaternion());
		return true;
		break;
	case EDMXRotationMode::RotationMode_Tilt:
		SceneComponentRef->SetRelativeRotation(FRotator(0.0f, 0.0f, outFloat).Quaternion());
		return true;
		break;
	default:
		return false;
		break;
	}
}

