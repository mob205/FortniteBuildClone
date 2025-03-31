// Fill out your copyright notice in the Description page of Project Settings.


#include "GridWorldSubsystem.h"
#include "AbilitySystem/Abilities/Build/PlacedStructure.h"
#include "GameplayTagContainer.h"

UGridWorldSubsystem::UGridWorldSubsystem()
{
	TagToType.Add(
		FGameplayTag::RequestGameplayTag("Abilities.Build.Structure.Floor"),
		EGridBuildingType::GRID_Floor);

	TagToType.Add(
		FGameplayTag::RequestGameplayTag("Abilities.Build.Structure.Ramp"),
		EGridBuildingType::GRID_Main);

	TagToType.Add(
		FGameplayTag::RequestGameplayTag("Abilities.Build.Structure.Pyramid"),
		EGridBuildingType::GRID_Main);
	
	TagToType.Add(
		FGameplayTag::RequestGameplayTag("Abilities.Build.Structure.Wall"),
		EGridBuildingType::GRID_Wall);
}

void UGridWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	Grid.Add(EGridBuildingType::GRID_Main, {});
	Grid.Add(EGridBuildingType::GRID_Wall, {});
	Grid.Add(EGridBuildingType::GRID_Floor, {});
}

void UGridWorldSubsystem::RegisterPlacedStructure(FIntVector GridPosition, APlacedStructure* Structure)
{
	FGameplayTag StructureTag = Structure->GetStructureTag();
	if (!IsOccupied(GridPosition, StructureTag))
	{
		EGridBuildingType GridType = TagToType[StructureTag];
		Grid[GridType].Add(GridPosition, Structure);
	}
}

bool UGridWorldSubsystem::IsOccupied(FIntVector GridPosition, FGameplayTag BuildingTag)
{
	EGridBuildingType GridType = TagToType[BuildingTag];
	return Grid[GridType].Contains(GridPosition);
}
