// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LexyVFXDMXBaseComponent.h"
#include "DMXRuntime/Public/DMXSubsystem.h"
#include "DMXRuntime/Public/Game/DMXComponent.h"
#include "DMXRuntime/Public/Library/DMXEntity.h"
#include "DMXRuntime/Public/Library/DMXEntityFixturePatch.h"
#include "DMXProtocol/Public/DMXProtocolTypes.h"
#include "LexyVFXDMXFunctionManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LEXYVFXCPPFIXTURES_API ULexyVFXDMXFunctionManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ULexyVFXDMXFunctionManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(Instanced, BlueprintReadWrite, EditAnywhere)
	UDMXComponent *DMXComp;

	UPROPERTY(Instanced, BlueprintReadWrite, EditAnywhere)
	UDMXEntityFixturePatch *Patch;

	UPROPERTY(Instanced, BlueprintReadWrite, EditAnywhere)
	TArray<ULexyVFXDMXBaseComponent*> LexyVFXFunctionComponents;

	UFUNCTION()
	virtual void SetParentDMXRef();

	UFUNCTION()
	void ReceivedDMX(FDMXProtocolName Protocol, int32 Universe, const TArray<uint8>& DMXBuffer);
};
