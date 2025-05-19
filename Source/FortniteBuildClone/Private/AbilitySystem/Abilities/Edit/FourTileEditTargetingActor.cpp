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
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Bitfield is %d."), InBitfield));

	// Every configuration, except 0, is a valid edit for all four-tile structures
	if (InBitfield == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("0 is not allowed - returning")));
		return;
	}

	int i{};
	for (; i < 4 && !EditMap->Contains(InBitfield); ++i)
	{
		InBitfield = RotateClockwise(InBitfield).BitField;
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Switching to %d"), InBitfield));
	}

	if (!EditMap->Contains(InBitfield))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Bitfield %d not in map!"), InBitfield));
	}
	// This should be a valid edit
	const float AngleRotated = -90 * i;
	AddActorWorldRotation({0, AngleRotated, 0});
	
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Rotating %f degrees CW"), AngleRotated));


	Super::SetSelectedEdit(InBitfield);
}
