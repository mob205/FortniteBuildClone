// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Edit/RampEditTargetingActor.h"

#include "AbilitySystem/Abilities/Edit/EditSelectionTile.h"

// "Perimeter" neighbors of each bit index when representing the bitfield as a 3x3 grid:
// 0 1 2
// 3 4 5
// 6 7 8
												 // 0  1  2  3   4  5  6  7  8
static constexpr int CounterClockwiseNeighbors[]  { 3, 0, 1, 6, -1, 2, 7, 8, 5 };
static constexpr int ClockwiseNeighbors[]		  { 1, 2, 5, 0, -1, 8, 3, 6, 7 };


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
		LastSelectedCornerIndex = TileBitIndex;
		bIsFirstTileSet = true;
		SelectedTiles.Set(TileBitIndex);
		UpdateSelectionTiles();
		return;
	}
	
	// Selected a corner
	// If the first selected tile is a corner, subsequent tiles can only be corners
	if (FirstTileIndex % 2 == 0 && TileBitIndex % 2 == 0)
	{
		// Get the direction (cw or ccw) from the newly selected corner to the previously selected corner
		const int* Direction = nullptr;

		if (LastSelectedCornerIndex == CounterClockwiseNeighbors[CounterClockwiseNeighbors[TileBitIndex]])
		{
			Direction = CounterClockwiseNeighbors;
		}
		else if (LastSelectedCornerIndex == ClockwiseNeighbors[ClockwiseNeighbors[TileBitIndex]])
		{
			Direction = ClockwiseNeighbors;
		}
		else
		{
			// Selected corner is diagonal from the last selected corner, which isn't allowed
			return;
		}

		// Select that side
		SelectedTiles.Set(Direction[TileBitIndex]);
		SelectedTiles.Set(TileBitIndex);

		int OldLastSelected = LastSelectedCornerIndex;
		LastSelectedCornerIndex = TileBitIndex;
		
		if (OldLastSelected == FirstTileIndex)
		{
			// This is the second corner selected, which determines the direction of the ramp
			// The first corner selected is always the lowest point, so it should be rotated to be on the "bottom" edge
			// Centerpiece to align to is the centerpiece that should be on the bottom edge, which is adjacent to first corner
			RotateToCenter(Direction[OldLastSelected]);
		}
		
		UpdateSelectionTiles();
		
	}
	// First tile was a centerpiece and any other tile was selected
	else if (FirstTileIndex % 2 == 1)
	{
		// Cannot select tiles immediately adjacent to the selected center
		if (CounterClockwiseNeighbors[FirstTileIndex] == TileBitIndex || ClockwiseNeighbors[FirstTileIndex] == TileBitIndex) { return; }

		// Fully set all of the bits - full ramp
		int32 FullRamp = (1 << 9) - 1;

		InternalUpdateEdit(FullRamp);
		
		SelectedTiles = FullRamp;
		UpdateSelectionTiles();

		// Rotate the ramp to match new direction
		RotateToCenter(FirstTileIndex);
	}
}

void ARampEditTargetingActor::EndSelecting()
{
	bIsFirstTileSet = false;
	SetSelectedEdit(CurrentEdit);
}

void ARampEditTargetingActor::SetSelectedEdit(int32 InBitfield)
{
	SelectedTiles = InBitfield;
	UpdateSelectionTiles();

	InternalUpdateEdit(InBitfield);
}

void ARampEditTargetingActor::RotateToCenter(int CenterIndex)
{
	// Maps centerpieces to number of clockwise turns
	static constexpr int NumCWTurns[]{2, 1, 3, 0};

	// Remapping selection indices               0  1  2  3  4  5  6  7  8
	static constexpr int ClockwiseMap[]			{6, 3, 0, 7, 4, 1, 8, 5, 2};
	static constexpr int CounterClockwiseMap[]	{2, 5, 8, 1, 4, 7, 0, 3, 6};
	
	int CurNumCWTurns = NumCWTurns[(CenterIndex - 1) / 2];
	float Yaw = 90 * CurNumCWTurns;
	AddActorLocalRotation({0, Yaw, 0});

	for (int i = 0; i < CurNumCWTurns; ++i)
	{
		FBitGrid TempBitGrid{};

		for (int j = 0; j < 9; ++j)
		{
			TempBitGrid.BitField |= static_cast<int>(SelectedTiles.Get(CounterClockwiseMap[j])) << j;
		}
		SelectedTiles = TempBitGrid;

		FirstTileIndex = ClockwiseMap[FirstTileIndex];
		LastSelectedCornerIndex = ClockwiseMap[LastSelectedCornerIndex];
	}
}

void ARampEditTargetingActor::InternalUpdateEdit(int32 InBitfield)
{
	CurrentEdit = InBitfield;

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

