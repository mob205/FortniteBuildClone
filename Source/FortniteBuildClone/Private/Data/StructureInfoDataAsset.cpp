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

TSubclassOf<AGhostPreviewStructure> UStructureInfoDataAsset::GetGhostClass(const FGameplayTag& StructureTag)
{
	FStructureClasses Classes;
	if (GetStructureClasses(StructureTag, Classes))
	{
		return Classes.TargetingActorClass;
	}
	return {};
}

TSubclassOf<APlacedStructure> UStructureInfoDataAsset::GetStructureActorClass(const FGameplayTag& StructureTag)
{
	FStructureClasses Classes;
	if (GetStructureClasses(StructureTag, Classes))
	{
		return Classes.StructureActorClass;
	}
	return {};
}

bool UStructureInfoDataAsset::GetStructureClasses(const FGameplayTag& StructureTag, FStructureClasses& Classes)
{
	if (!bHasInitializedMaps) { InitializeMaps(); }

	if (!StructureClasses.Contains(StructureTag))
	{
		Classes = {};
		return false;
	}

	Classes = StructureClasses[StructureTag];
	return true;
}

TSubclassOf<UPlacementStrategy> UStructureInfoDataAsset::GetPlacementStrategyClass(const FGameplayTag& StructureTag)
{
	if (!bHasInitializedMaps) { InitializeMaps(); }

	checkf(StrategyClasses.Contains(StructureTag),
		TEXT("Attempted to find placement strategy of invalid structure tag %s."),
		*StructureTag.GetTagName().ToString());
	
	return StrategyClasses[StructureTag];
}

void UStructureInfoDataAsset::InitializeMaps()
{
	for (const auto& Structure : StructureInfo)
	{
		StructureClasses.Add(Structure.GameplayTag, Structure.StructureClasses);
		StrategyClasses.Add(Structure.GameplayTag, Structure.PlacementStrategyClass);
	}
	bHasInitializedMaps = true;
}
