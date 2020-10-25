// Fill out your copyright notice in the Description page of Project Settings.


#include "LexyVFXDMXFunctionManager.h"

// Sets default values for this component's properties
ULexyVFXDMXFunctionManager::ULexyVFXDMXFunctionManager()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULexyVFXDMXFunctionManager::BeginPlay()
{
	Super::BeginPlay();
	this->SetParentDMXRef();
	Patch = DMXComp->GetFixturePatch();
	// ...
	
}


// Called every frame
void ULexyVFXDMXFunctionManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void ULexyVFXDMXFunctionManager::SetParentDMXRef()
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

	Patch = DMXComp->GetFixturePatch();
	if (Patch)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found Fixture Patch on DMX Component: %s"), *Patch->Name);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't find valid DMX Patch on DMX Component"));
	}
}

void ULexyVFXDMXFunctionManager::ReceivedDMX(FDMXProtocolName Protocol, int32 Universe, const TArray<uint8>& DMXBuffer)
{
}