// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Edit/FourTileEditTargetingActor.h"

// Rotates 2x2 tile bitfield clockwise
// 0 1   =====>   2 0
// 2 3   rotate   3 1
//
static constexpr FBitGrid RotateClockwise(FBitGrid InBitGrid)
{
	                       // 0  1  2  3
	constexpr int Mapping[] { 2, 0, 3, 1 };

	FBitGrid OutGrid{};

	for (int i = 0; i < 4; ++i)
	{
		OutGrid.BitField |= InBitGrid.Get(Mapping[i]) << i;
	}

	return OutGrid;
}

void AFourTileEditTargetingActor::SetSelectedEdit(int32 InBitfield)
{
	// Every configuration, except 0, is a valid edit for all four-tile structures
	if (InBitfield == 0)
	{
		return;
	}

	// Rotate to find the right orientation, since edit map only defines one orientation per configuration
	int i{};
	for (; i < 4 && !EditMap->Contains(InBitfield); ++i)
	{
		InBitfield = RotateClockwise(InBitfield).BitField;
	}
	
	// This should be a valid edit
	// Rotate structure opposite way of bitfield rotation to retain selection orientation 
	const float AngleRotated = -90 * i;
	AddActorWorldRotation({0, AngleRotated, 0});
	
	Super::SetSelectedEdit(InBitfield);
}
