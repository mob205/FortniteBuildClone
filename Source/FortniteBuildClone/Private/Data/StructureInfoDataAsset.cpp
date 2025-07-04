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

UStaticMesh* UStructureInfoDataAsset::GetMesh(const FGameplayTag& StructureTag) const
{
	checkf(Meshes.Contains(StructureTag),
		TEXT("Attempted to find mesh of invalid structure tag %s."),
		*StructureTag.GetTagName().ToString());
	
	return Meshes[StructureTag];
}

TSubclassOf<APlacedStructure> UStructureInfoDataAsset::GetStructureActorClass(const FGameplayTag& StructureTag) const
{
	checkf(StructureClasses.Contains(StructureTag),
		TEXT("Attempted to find structure class of invalid structure tag %s."),
		*StructureTag.GetTagName().ToString());
	
	return StructureClasses[StructureTag];
}

TSubclassOf<UPlacementStrategy> UStructureInfoDataAsset::GetPlacementStrategyClass(const FGameplayTag& StructureTag) const
{

	checkf(StrategyClasses.Contains(StructureTag),
		TEXT("Attempted to find placement strategy of invalid structure tag %s."),
		*StructureTag.GetTagName().ToString());
	
	return StrategyClasses[StructureTag];
}

TMap<FGameplayTag, TSubclassOf<UPlacementStrategy>> UStructureInfoDataAsset::GetAllPlacementStrategyClasses() const
{
	return StrategyClasses;
}

const UEditMapDataAsset* UStructureInfoDataAsset::GetEditMapAsset(const FGameplayTag& StructureTag) const
{
	checkf(EditMaps.Contains(StructureTag),
		TEXT("Attempted to find edit map of invalid structure tag %s."),
		*StructureTag.GetTagName().ToString());
		
	return EditMaps[StructureTag];

}

FEditTargetingClassInfo UStructureInfoDataAsset::GetEditTargetingClass(const FGameplayTag& StructureTag) const
{
	checkf(EditTargetingActors.Contains(StructureTag),
		TEXT("Attempted to find edit map of invalid structure tag %s."),
		*StructureTag.GetTagName().ToString());
	
	return EditTargetingActors[StructureTag];
}

int32 UStructureInfoDataAsset::GetDefaultEdit(const FGameplayTag& StructureTag) const
{
	return GetEditMapAsset(StructureTag)->GetDefaultEdit();
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
