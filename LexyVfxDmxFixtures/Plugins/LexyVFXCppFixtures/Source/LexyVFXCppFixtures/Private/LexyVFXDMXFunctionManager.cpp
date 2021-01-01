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
	SetFunctionComponentReferences();
	static UDMXSubsystem* UnrealDMXSubsystem = UDMXSubsystem::GetDMXSubsystem_Pure();
	ReceivedDMX.BindUFunction(this, "ProcessDMX");
	UnrealDMXSubsystem->OnProtocolReceived.Add(ULexyVFXDMXFunctionManager::ReceivedDMX);
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

	if (DMXComp->IsValidLowLevel())
		Patch = DMXComp->GetFixturePatch();
	if (!Patch)
		UE_LOG(LogTemp, Warning, TEXT("Couldn't find valid DMX Patch on DMX Component"));
}

void ULexyVFXDMXFunctionManager::SetFunctionComponentReferences()
{
	TArray<UActorComponent*> actorComponents = this->GetOwner()->GetComponentsByClass(ULexyVFXDMXBaseComponent::StaticClass());
	LexyVFXFunctionComponents.Empty();
	for (UActorComponent* actorcomponent : actorComponents)
	{
		ULexyVFXDMXBaseComponent* localcomp = Cast<ULexyVFXDMXBaseComponent>(actorcomponent);
		if (localcomp->IsValidLowLevel())
			LexyVFXFunctionComponents.AddUnique(localcomp);
	}
}

void ULexyVFXDMXFunctionManager::ProcessDMX(FDMXProtocolName Protocol, int32 Universe, const TArray<uint8>& DMXBuffer)
{
	//UE_LOG(LogTemp, Warning, TEXT("Processing DMX..."));

	TMap<FName, int32> NImapDMXFunctionValues;
	TArray<FName> LocalKeys;
	static UDMXSubsystem* UnrealDMXSubsystem = UDMXSubsystem::GetDMXSubsystem_Pure();
	UnrealDMXSubsystem->GetFunctionsMap(Patch, Protocol, NImapDMXFunctionValues);
	NImapDMXFunctionValues.GetKeys(LocalKeys);

	for (ULexyVFXDMXBaseComponent* functionComponent : LexyVFXFunctionComponents)
	{
		functionComponent->UpdateDMX(NImapDMXFunctionValues, LocalKeys);
	}
}