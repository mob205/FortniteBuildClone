// Fill out your copyright notice in the Description page of Project Settings.


#include "FBCGameState.h"

#include "Net/UnrealNetwork.h"
#include "Structure/PlacedStructure.h"

void AFBCGameState::DisableStructures_Implementation(const TArray<APlacedStructure*>& Structures)
{
	for (const auto Structure : Structures)
	{
		if (IsValid(Structure))
		{
			Structure->DisableStructure();
		}
	}
}