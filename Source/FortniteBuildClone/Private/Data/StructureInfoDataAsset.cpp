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

UStaticMesh* UStructureInfoDataAsset::GetMesh(const FGameplayTag& StructureTag)
{
	checkf(Meshes.Contains(StructureTag),
		TEXT("Attempted to find mesh of invalid structure tag %s."),
		*StructureTag.GetTagName().ToString());
	
	return Meshes[StructureTag];
}

TSubclassOf<APlacedStructure> UStructureInfoDataAsset::GetStructureActorClass(const FGameplayTag& StructureTag)
{
	
	checkf(StructureClasses.Contains(StructureTag),
		TEXT("Attempted to find structure class of invalid structure tag %s."),
		*StructureTag.GetTagName().ToString());
	
	return StructureClasses[StructureTag];
}

TSubclassOf<UPlacementStrategy> UStructureInfoDataAsset::GetPlacementStrategyClass(const FGameplayTag& StructureTag)
{

	checkf(StrategyClasses.Contains(StructureTag),
		TEXT("Attempted to find placement strategy of invalid structure tag %s."),
		*StructureTag.GetTagName().ToString());
	
	return StrategyClasses[StructureTag];
}

TMap<FGameplayTag, TSubclassOf<UPlacementStrategy>> UStructureInfoDataAsset::GetAllPlacementStrategyClasses()
{
	return StrategyClasses;
}

const UEditMapDataAsset* UStructureInfoDataAsset::GetEditMapAsset(const FGameplayTag& StructureTag)
{
	if (EditMaps.Contains(StructureTag))
	{
		return EditMaps[StructureTag];
	}
	return nullptr;
}

FEditTargetingClassInfo UStructureInfoDataAsset::GetEditTargetingClass(const FGameplayTag& StructureTag)
{
	if (EditTargetingActors.Contains(StructureTag))
	{
		return EditTargetingActors[StructureTag];
	}
	return {};
}

void UStructureInfoDataAsset::PostLoad()
{
	Super::PostLoad();

	InitializeMaps();
}

void UStructureInfoDataAsset::InitializeMaps()
{
	for (const auto& Structure : StructureInfo)
	{
		StrategyClasses.Add(Structure.GameplayTag, Structure.PlacementStrategyClass);
		StructureClasses.Add(Structure.GameplayTag, Structure.StructureClass);
		Meshes.Add(Structure.GameplayTag, Structure.Mesh);
	}
}
