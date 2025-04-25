// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Edit/EditTargetingActor.h"

#include "AbilitySystem/Abilities/Edit/EditSelectionTile.h"
#include "FBCBlueprintLibrary.h"
#include "AbilitySystem/Abilities/Edit/EditTargetData.h"
#include "Data/EditMapDataAsset.h"

AEditTargetingActor::AEditTargetingActor()
{
	PrimaryActorTick.bCanEverTick = true;

	GhostMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Ghost Mesh Component");
	GhostMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GhostMeshComponent->SetGenerateOverlapEvents(false);
	SetRootComponent(GhostMeshComponent);
}

void AEditTargetingActor::RegisterSelectionTile(AEditSelectionTile* SelectionTile, int BitIndex)
{
	SelectionTile->SetEditBitfieldIndex(BitIndex);
	SelectionTiles.Add(SelectionTile);
}

void AEditTargetingActor::InitializeEditTargeting(APlayerController* PC, float Range, const FEditMap* InEditMap)
{
	AvatarPC = PC;
	TargetingRange = Range;
	EditMap = InEditMap;
}

void AEditTargetingActor::ConfirmTargetingAndContinue()
{
	FGameplayAbilityTargetDataHandle Handle{new FEditTargetData{CurrentEdit}};
	TargetDataReadyDelegate.Broadcast(Handle);
}

void AEditTargetingActor::UpdateSelectionTiles()
{
	bool bIsValidEdit = EditMap->Contains(CurrentEdit);
	for (const auto Tile : SelectionTiles)
	{
		int BitIdx = Tile->GetEditBitfieldIndex();

		Tile->SetValidity(bIsValidEdit);
		Tile->SetSelection(CurrentEdit & (1 << BitIdx));
	}
}

AEditSelectionTile* AEditTargetingActor::GetCurrentTile() const
{
	FHitResult HitResult{};

	if (UFBCBlueprintLibrary::TraceControllerLook(AvatarPC, TargetingRange, HitResult))
	{
		AEditSelectionTile* AsSelectionTile = Cast<AEditSelectionTile>(HitResult.GetActor());
		if (IsValid(AsSelectionTile) && SelectionTiles.Contains(AsSelectionTile))
		{
			return AsSelectionTile;
		}
	}
	return nullptr;
}

void AEditTargetingActor::ProcessSelecting()
{
	if (AEditSelectionTile* Tile = GetCurrentTile())
	{
		if (EncounteredTiles.Contains(Tile)) { return; }
		
		int Bitmask = 1 << Tile->GetEditBitfieldIndex();

		bool bIsTileSelected = CurrentEdit & Bitmask;
		
		if (EncounteredTiles.IsEmpty())
		{
			bFirstTileInitialStatus = bIsTileSelected;
		}
		// While holding to select, tiles we flip should have the same starting state as the first one we encounter
		if (bFirstTileInitialStatus == bIsTileSelected)
		{
			CurrentEdit ^= (Bitmask);
			Tile->SetSelection(!bIsTileSelected);
		}
		EncounteredTiles.Add(Tile);
	}
}

void AEditTargetingActor::EndSelecting()
{
	bIsSelecting = false;
	EncounteredTiles.Reset();
	SetSelectedEdit(CurrentEdit);
}

void AEditTargetingActor::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);
	GhostMeshComponent->SetMaterial(0, GhostMaterial);
	UpdateSelectionTiles();
}

void AEditTargetingActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsSelecting)
	{
		ProcessSelecting();
	}
	if (!bIsSelecting && bLastSelecting)
	{
		EndSelecting();
	}
	bLastSelecting = bIsSelecting;
}

void AEditTargetingActor::SetSelectedEdit(int32 InBitfield)
{
	CurrentEdit = InBitfield;

	UpdateSelectionTiles();
	
	if (EditMap->Contains(CurrentEdit))
	{
		GhostMeshComponent->SetStaticMesh(EditMap->Find(CurrentEdit)->EditPreviewMesh);
	}
}

