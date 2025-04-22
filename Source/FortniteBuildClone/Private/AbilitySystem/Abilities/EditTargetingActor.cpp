// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/EditTargetingActor.h"


AEditTargetingActor::AEditTargetingActor()
{
	PrimaryActorTick.bCanEverTick = false;

	GhostMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Ghost Mesh Component");
	GhostMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GhostMeshComponent->SetGenerateOverlapEvents(false);
	SetRootComponent(GhostMeshComponent);
}

void AEditTargetingActor::BeginPlay()
{
	Super::BeginPlay();
	GhostMeshComponent->SetMaterial(0, GhostMaterial);
}

void AEditTargetingActor::SetSelectedEdit(int32 InBitfield)
{
	if (InBitfield == SelectedEdit) { return; }

	SelectedEdit = InBitfield;

	if (EditMap.Contains(SelectedEdit))
	{
		GhostMeshComponent->SetStaticMesh(EditMap[SelectedEdit].EditPreviewMesh);
	}
}

bool AEditTargetingActor::GetSelectedEdit(TSubclassOf<APlacedStructure>& OutStructureClass) const
{
	if (EditMap.Contains(SelectedEdit))
	{
		OutStructureClass = EditMap[SelectedEdit].StructureClass;
		return true;
	}
	return false;
}

