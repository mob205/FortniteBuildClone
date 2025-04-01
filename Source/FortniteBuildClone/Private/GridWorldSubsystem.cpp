// Fill out your copyright notice in the Description page of Project Settings.


#include "GridWorldSubsystem.h"

#include "FBCBlueprintLibrary.h"
#include "Build/PlacedStructure.h"
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
		EGridBuildingType::GRID_NorthWall);
}

void UGridWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);
}

void UGridWorldSubsystem::RegisterPlacedStructure(APlacedStructure* Structure)
{
	FGridStructureInfo Info = GetGridStructureInfo(Structure);
	
	if (GetStructureAtPosition(Info.GridLocation, Info.BuildingType) != nullptr)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("Attempted to register a structure at a location with a pre-existing structure!"));
		return;
	}

	if (!Grid.Contains(Info.GridLocation))
	{
		Grid.Add(Info.GridLocation, {});
	}
	Grid[Info.GridLocation][static_cast<int>(Info.BuildingType)] = Structure;
}

bool UGridWorldSubsystem::IsOccupied(const FTransform& Transform, FGameplayTag StructureTag)
{
	const FIntVector GridLocation = UFBCBlueprintLibrary::GetGridCoordinateLocation(Transform.GetLocation());
	const int Yaw = UFBCBlueprintLibrary::SnapAngleToGridInt(Transform.Rotator().Yaw);
	EGridBuildingType BuildingType = TagToType[StructureTag];
	
	return GetStructureAtPosition(GridLocation, BuildingType, Yaw % 180 != 0) != nullptr;
}

APlacedStructure* UGridWorldSubsystem::GetStructureAtPosition(FIntVector GridPosition, EGridBuildingType BuildingType,
                                                              bool bIsEastWall)
{
	if (!Grid.Contains(GridPosition)) { return nullptr; }
	if (BuildingType == EGridBuildingType::GRID_NorthWall && bIsEastWall)
	{
		BuildingType = EGridBuildingType::GRID_EastWall;
	}
	return Grid[GridPosition][static_cast<int>(BuildingType)];
}

FGridStructureInfo UGridWorldSubsystem::GetGridStructureInfo(APlacedStructure* Structure)
{
	FGridStructureInfo Result{};
	
	FGameplayTag StructureTag = Structure->GetStructureTag();
	EGridBuildingType BuildingType = TagToType[StructureTag];
	
	Result.GridLocation = UFBCBlueprintLibrary::GetGridCoordinateLocation(Structure->GetActorLocation());

	// Each grid slot can have two walls, so verify which wall it is
	if (BuildingType == EGridBuildingType::GRID_NorthWall)
	{
		const int Yaw = UFBCBlueprintLibrary::SnapAngleToGridInt(Structure->GetActorRotation().Yaw);

		if (Yaw % 180 != 0)
		{
			BuildingType = EGridBuildingType::GRID_EastWall;
		}
	}
	Result.BuildingType = BuildingType;
	return Result;
}


