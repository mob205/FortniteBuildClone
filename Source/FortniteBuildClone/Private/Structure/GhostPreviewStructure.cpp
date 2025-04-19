// Fill out your copyright notice in the Description page of Project Settings.


#include "Structure/GhostPreviewStructure.h"

AGhostPreviewStructure::AGhostPreviewStructure()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	Mesh->SetupAttachment(Root);

	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->SetGenerateOverlapEvents(false);
}

void AGhostPreviewStructure::SetMaterial(UMaterialInterface* Material)
{
	if (Mesh->GetMaterial(0) != Material)
	{
		Mesh->SetMaterial(0, Material);
	}
}




