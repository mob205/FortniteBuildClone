// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/StructureInfoDataAsset.h"

FGameplayTag UStructureInfoDataAsset::GetTagFromInput(const UInputAction* InputAction) const
{
	// Can reimplement this as a map if needed
	// Since there should only be 4 structures, shouldn't be necessary
	for (const auto& Structure : StructureInfo)
	{
		if (Structure.InputAction == InputAction)
		{
			return Structure.GameplayTag;
		}
	}
	return {};
}

bool UStructureInfoDataAsset::GetStructureClasses(const FGameplayTag& GameplayTag, FStructureClasses& Classes)
{
	if (!bHasInitializedStructureClasses) { InitializeStructureClasses(); }

	if (!StructureClasses.Contains(GameplayTag))
	{
		Classes = {};
		return false;
	}

	Classes = StructureClasses[GameplayTag];
	return true;
}

void UStructureInfoDataAsset::InitializeStructureClasses()
{
	for (const auto& Structure : StructureInfo)
	{
		StructureClasses.Add(Structure.GameplayTag, Structure.StructureClasses);
	}
	bHasInitializedStructureClasses = true;
}
