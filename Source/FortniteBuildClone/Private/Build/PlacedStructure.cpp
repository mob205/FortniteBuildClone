// Fill out your copyright notice in the Description page of Project Settings.


#include "Build/PlacedStructure.h"

#include "GridWorldSubsystem.h"
#include "Net/UnrealNetwork.h"

APlacedStructure::APlacedStructure()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APlacedStructure::BeginPlay()
{
	Super::BeginPlay();

	GridWorldSubsystem = GetWorld()->GetSubsystem<UGridWorldSubsystem>();
	GridWorldSubsystem->RegisterPlacedStructure(this);
}

void APlacedStructure::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APlacedStructure, StructureTag, COND_None);

}

