// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/LightComponent.h"
#include "Components/SpotLightComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "DMXRuntime/Public/DMXSubsystem.h"
#include "DMXRuntime/Public/Game/DMXComponent.h"
#include "DMXRuntime/Public/Library/DMXEntity.h"
#include "DMXRuntime/Public/Library/DMXEntityFixturePatch.h"
#include "LexyVFXDMXBaseComponent.generated.h"

USTRUCT(BlueprintType)
struct FDMXComponentFunctions
{
	GENERATED_BODY()

		UPROPERTY()
		TArray<FName> nDMXComponentFunctions;
};

UENUM(BlueprintType)
enum class EDMXParameterBitDepth : uint8
{
	BitDepth_8bits	UMETA(DisplayName = "One Channel"),
	BitDepth_16bits	UMETA(DisplayName = "Two Channels"),
	BitDepth_24bits	UMETA(DisplayName = "Three Channels")
};

UENUM(BlueprintType)
enum class EDMXRotationMode : uint8
{
	RotationMode_Pan	UMETA(DisplayName = "Pan"),
	RotationMode_Tilt	UMETA(DisplayName = "Tilt")
};

UCLASS( Abstract, ClassGroup = (DMXFunctions), meta = (BlueprintSpawnableComponent) )
class LEXYVFXCPPFIXTURES_API ULexyVFXDMXBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULexyVFXDMXBaseComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(Instanced, BlueprintReadWrite, EditAnywhere)
		UDMXComponent *DMXComp;

	UPROPERTY(Instanced, BlueprintReadWrite, EditAnywhere)
		UDMXEntity *Patch;

	UPROPERTY(EditAnywhere)
		FDMXComponentFunctions FunctionNames;

	UFUNCTION()
		virtual void SetParentDMXRef();

	UFUNCTION(BlueprintCallable)
		virtual void InitDMXFunctionNames(TArray<FName> DMXFunctionNames);

	UFUNCTION(BlueprintCallable)
		virtual TArray<UActorComponent*> FindComponentsByName(TSubclassOf<UActorComponent> ComponentType, TArray<FString> searchNames);

	UFUNCTION(BlueprintCallable)
		virtual void UpdateDMX(TMap<FName, int32> NImapDMXFunctionValues, TArray<FName> nDMXComponentFunctions);

	UFUNCTION(BlueprintCallable)
		virtual void UpdateDMXMaterialScalarParameter(UMaterialInstanceDynamic *miTargetMaterial, EDMXParameterBitDepth DMXBitDepth, FName nMaterialParameterName, float fScaleFactor, float fRangeMin, float fRangeMax, TMap<FName, int32> NImapDMXFunctionValues, FName nDMXComponentFunction);

	UFUNCTION(BlueprintCallable)
		virtual void UpdateDMXMaterialVectorParameter(UMaterialInstanceDynamic *miTargetMaterial, EDMXParameterBitDepth DMXBitDepth, FName nMaterialParameterName, TMap<FName, int32> NImapDMXFunctionValues, TArray<FName> nDMXComponentFunctions);

	UFUNCTION(BlueprintCallable)
		virtual void UpdateDMXLightColor(EDMXParameterBitDepth DMXBitDepth, ULightComponent *LightComponentRef, float fRange, TMap<FName, int32> NImapDMXFunctionValues, TArray<FName> nDMXComponentFunctions);

	UFUNCTION(BlueprintCallable)
		virtual void UpdateDMXSpringArm(EDMXParameterBitDepth DMXBitDepth, USpringArmComponent *SpringArmComponentRef, float fRange, TMap<FName, int32> NImapDMXFunctionValues, FName nDMXComponentFunction);

	UFUNCTION(BlueprintCallable)
		virtual bool UpdateDMXSpotConeAngle(EDMXParameterBitDepth DMXBitDepth, ULightComponent *LightComponentRef, float fBeamRangeMax, float fBeamRangeMin, TMap<FName, int32> NImapDMXFunctionValues, FName nDMXComponentFunction);

	UFUNCTION(BlueprintCallable)
		virtual void UpdateDMXLightIntensity(EDMXParameterBitDepth DMXBitDepth, ULightComponent *LightComponentRef, float fRange, TMap<FName, int32> NImapDMXFunctionValues, FName nDMXComponentFunction);

	UFUNCTION(BlueprintCallable)
		virtual bool UpdateDMXRotation(EDMXParameterBitDepth DMXBitDepth, USceneComponent *SceneComponentRef, EDMXRotationMode eRotationMode, float fRange, TMap<FName, int32> NImapDMXFunctionValues, FName nDMXComponentFunction);
};
