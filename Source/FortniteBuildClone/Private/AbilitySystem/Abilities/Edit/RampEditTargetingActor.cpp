// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Edit/RampEditTargetingActor.h"

#include "AbilitySystem/Abilities/Edit/EditSelectionTile.h"

void ARampEditTargetingActor::StartSelecting()
{
	SelectedTiles = 0;
}

void ARampEditTargetingActor::ProcessSelecting()
{
	// No tile was selected
	AEditSelectionTile* Tile = GetCurrentTile();
	if (!Tile) { return; }

	int TileBitIndex = Tile->GetEditBitfieldIndex();

	// Tile is already selected
	if (SelectedTiles.Get(TileBitIndex)) { return; }

	// First tile selected
	if (!bIsFirstTileSet)
	{
		FirstTileIndex = TileBitIndex;
		bIsFirstTileSet = true;
		SelectedTiles.Set(TileBitIndex);
		UpdateSelectionTiles();
		return;
	}
	
	// Selected a corner
	// If the first selected tile is a corner, subsequent tiles can only be corners
	if (FirstTileIndex % 2 == 0 && TileBitIndex % 2 == 0)
	{
		// A corner must be nearby another corner
		if (!SelectedTiles.Get((TileBitIndex - 2) % 8) && !SelectedTiles.Get((TileBitIndex + 2) % 8)) { return; }
	}
	// First tile was a centerpiece and any other tile was selected
	else if (FirstTileIndex % 2 == 1)
	{
											 // 0  1  2  3   4  5  6  7  8
		static constexpr int LeftNeighbors[]  { 3, 0, 1, 6, -1, 2, 7, 8, 5 };
		static constexpr int RightNeighbors[] { 1, 2, 5, 0, -1, 8, 3, 6, 7 };
		
		// Cannot select tiles immediately adjacent to the selected center
		if (LeftNeighbors[FirstTileIndex] == TileBitIndex || RightNeighbors[FirstTileIndex] == TileBitIndex) { return; }

		// Fully set all of the bits - full ramp
		int32 FullRamp = (1 << 9) - 1;
		SetSelectedEdit(FullRamp); 
		SelectedTiles = FullRamp;

		// Rotate the ramp to match new direction
		static constexpr int CWTurns[]{2, 1, 3, 0};
		
		float Yaw = 90 * CWTurns[(FirstTileIndex - 1) / 2];
		AddActorLocalRotation({0, Yaw, 0});
	}
}

void ARampEditTargetingActor::EndSelecting()
{
	bIsFirstTileSet = false;
	SetSelectedEdit(CurrentEdit);
}

void ARampEditTargetingActor::SetSelectedEdit(int32 InBitfield)
{
	CurrentEdit = InBitfield;

	SelectedTiles = InBitfield;

	UpdateSelectionTiles();
	
	if (EditMap->Contains(CurrentEdit))
	{
		GhostMeshComponent->SetStaticMesh(EditMap->Find(CurrentEdit)->EditPreviewMesh);
	}
}

void ARampEditTargetingActor::UpdateSelectionTiles()
{
	for (const auto Tile : SelectionTiles)
	{
		int BitIdx = Tile->GetEditBitfieldIndex();

		Tile->SetValidity(true);
		Tile->SetSelection(SelectedTiles.Get(BitIdx));
	}
}
