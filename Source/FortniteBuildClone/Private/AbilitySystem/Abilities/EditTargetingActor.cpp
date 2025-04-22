// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/EditTargetingActor.h"

#include "FBCBlueprintLibrary.h"
#include "FortniteBuildClone/FortniteBuildClone.h"


AEditTargetingActor::AEditTargetingActor()
{
	PrimaryActorTick.bCanEverTick = true;

	GhostMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Ghost Mesh Component");
	GhostMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GhostMeshComponent->SetGenerateOverlapEvents(false);
	SetRootComponent(GhostMeshComponent);
}

void AEditTargetingActor::InitializeEditTargeting(APlayerController* PC, float Range)
{
	AvatarPC = PC;
	TargetingRange = Range;
}

void AEditTargetingActor::StartSelecting()
{
	UE_LOG(LogFBC, Display, TEXT("Start selecting."));

	FHitResult HitResult{};
	if (UFBCBlueprintLibrary::TraceControllerLook(AvatarPC, TargetingRange, HitResult))
	{
		
	}
}

void AEditTargetingActor::ContinueSelecting()
{
	UE_LOG(LogFBC, Display, TEXT("Continue selecting."));
}

void AEditTargetingActor::EndSelecting()
{
	UE_LOG(LogFBC, Display, TEXT("End selecting."));
}

void AEditTargetingActor::BeginPlay()
{
	Super::BeginPlay();
	GhostMeshComponent->SetMaterial(0, GhostMaterial);
}

void AEditTargetingActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsSelecting && !bLastSelecting)
	{
		StartSelecting();
	}
	if (!bIsSelecting && bLastSelecting)
	{
		EndSelecting();
	}
	if (bIsSelecting && bLastSelecting)
	{
		ContinueSelecting();
	}
	bLastSelecting = bIsSelecting;
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

