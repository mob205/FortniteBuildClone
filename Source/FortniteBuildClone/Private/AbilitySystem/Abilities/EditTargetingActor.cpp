// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/EditTargetingActor.h"


AEditTargetingActor::AEditTargetingActor()
{
	PrimaryActorTick.bCanEverTick = true;

	GhostMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Ghost Mesh Component");
	GhostMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GhostMeshComponent->SetGenerateOverlapEvents(false);
	SetRootComponent(GhostMeshComponent);
}


void AEditTargetingActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

