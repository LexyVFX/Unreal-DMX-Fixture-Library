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

	// Using OnProtocolReceived_Deprecated until the DMXComponent's OnPatchReceived is made Public in 4.27
	//UnrealDMXSubsystem->OnProtocolReceived_DEPRECATED.Add(ULexyVFXDMXFunctionManager::ReceivedDMX);
	
	//Switching to OnFixturePatchReceived pending it being made public
	DMXComp->OnFixturePatchReceived.Add(ULexyVFXDMXFunctionManager::ReceivedDMX);
	
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
	TArray<UActorComponent*> ActorComponents;
	this->GetOwner()->GetComponents(ULexyVFXDMXBaseComponent::StaticClass(), ActorComponents, false);

	LexyVFXFunctionComponents.Empty();
	for (UActorComponent* ActorComponent : ActorComponents)
	{
		ULexyVFXDMXBaseComponent* LocalComp = Cast<ULexyVFXDMXBaseComponent>(ActorComponent);
		if (LocalComp->IsValidLowLevel())
			LexyVFXFunctionComponents.AddUnique(LocalComp);
	}
}

void ULexyVFXDMXFunctionManager::ProcessDMX(FDMXProtocolName Protocol, int32 Universe, const TArray<uint8>& DMXBuffer)
{
	TMap<FName, int32> NImapDMXFunctionValues;
	TMap<FDMXAttributeName, int32> DImapDMXFunctionValues;
	TArray<FName> LocalKeys;
	static UDMXSubsystem* UnrealDMXSubsystem = UDMXSubsystem::GetDMXSubsystem_Pure();
	UnrealDMXSubsystem->GetFunctionsMap(Patch, DImapDMXFunctionValues);
	NImapDMXFunctionValues.GetKeys(LocalKeys);

	for (ULexyVFXDMXBaseComponent* FunctionComponent : LexyVFXFunctionComponents)
	{
		FunctionComponent->UpdateDMX(DImapDMXFunctionValues, LocalKeys);
	}
}