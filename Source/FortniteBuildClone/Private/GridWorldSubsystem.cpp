// Fill out your copyright notice in the Description page of Project Settings.


#include "GridWorldSubsystem.h"

#include "FBCBlueprintLibrary.h"
#include "Build/PlacedStructure.h"
#include "GameplayTagContainer.h"
#include "GridSizes.h"
#include "Kismet/BlueprintInstancedStructLibrary.h"


const TStaticArray<FIntVector, 4> Directions
{ 
	FIntVector{1, 0,0}, // North
	FIntVector{0, 1,0}, // East
	FIntVector{-1,0,0}, // South
	FIntVector{0,-1,0} // West
};

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
}

void UGridWorldSubsystem::RegisterPlacedStructure(APlacedStructure* Structure)
{
	FGridStructureInfo Info = GetGridStructureInfo(Structure);
	
	// Add new slot before querying for structure so we can find where to store new structure
	if (!Grid.Contains(Info.GridLocation))
	{
		Grid.Add(Info.GridLocation);
	}

	APlacedStructure** StructureLocation = FindStructureAtPosition(Info.GridLocation, Info.BuildingType, Structure->GetActorRotation());
	
	if (StructureLocation == nullptr)
	{
		// Couldn't find grid slot. This shouldn't happen since it was just added
		UE_LOG(LogTemp, Warning,
			TEXT("Attempted to register a structure at an invalid grid slot!"));
		return;
	}
	
	if (*StructureLocation != nullptr)
	{
		UE_LOG(LogTemp, Warning,
			TEXT("Attempted to register a structure at a location with a pre-existing structure!"));
		return;

		// Maybe clean up newly added grid slot since if it's unused?
	}

	// Store passed structure into the found spot
	*StructureLocation = Structure;
}

bool UGridWorldSubsystem::IsOccupied(const FTransform& Transform, FGameplayTag StructureTag)
{
	const FIntVector GridLocation = UFBCBlueprintLibrary::GetGridCoordinateLocation(Transform.GetLocation());

	if (!TagToType.Contains(StructureTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("Structure tag not found!"));
		return true;
	}
	EGridBuildingType BuildingType = TagToType[StructureTag];

	APlacedStructure** StructureLocation = FindStructureAtPosition(GridLocation, BuildingType, Transform.Rotator());
	// StructureLocation == nullptr means invalid grid was queried
	// *StructureLocation == nullptr means grid is valid, but no specified structure wasn't present
	return StructureLocation != nullptr && *StructureLocation != nullptr;
}

APlacedStructure** UGridWorldSubsystem::FindStructureAtPosition(const FIntVector& GridPosition, EGridBuildingType BuildingType,
	const FRotator& WallRotation)
{
	
	if (BuildingType == EGridBuildingType::GRID_Wall)
	{
		return FindWallStructureAtPosition(GridPosition, GetWallDirection(WallRotation));
	}
	else
	{
		return FindMainStructureAtPosition(GridPosition, BuildingType);
	}
}

APlacedStructure** UGridWorldSubsystem::FindMainStructureAtPosition(const FIntVector& GridPosition,
	EGridBuildingType BuildingType)
{
	if (!Grid.Contains(GridPosition)) { return nullptr; }
	
	switch (BuildingType)
	{
	case EGridBuildingType::GRID_Floor:
		return &Grid[GridPosition].Floor;

	case EGridBuildingType::GRID_Main:
		return &Grid[GridPosition].Main;

	default:
		UE_LOG(LogTemp, Error, TEXT("GridWorldSubsystem: Attempted to get invalid main structure."));
		return nullptr;
	}
}

APlacedStructure** UGridWorldSubsystem::FindWallStructureAtPosition(const FIntVector& GridPosition, EWallDirection WallDirection)
{
	const int WallDirectionInt = static_cast<int>(WallDirection);

	// Each wall can have two representations in the coordinate grid
	// First, check the alternative representation (the one other than GridPosition)
	// Alternative representation is in adjacent grid slot
	FIntVector AltGridPosition = GridPosition + Directions[WallDirectionInt];;

	if (Grid.Contains(AltGridPosition))
	{
		// Direction of wall is opposite relative to the adjacent grid
		int EquivalentWallDirection = (WallDirectionInt + 2) % 4;
		return &Grid[AltGridPosition].Walls[EquivalentWallDirection];
	}

	// No alternative grid slot. So, just return the one provided
	// Do this one last since we want to return this one if both representations are nullptr
	if (Grid.Contains(GridPosition))
	{
		return &Grid[GridPosition].Walls[WallDirectionInt];
	}
	
	// Neither grid slot existed
	return nullptr;
}

FGridStructureInfo UGridWorldSubsystem::GetGridStructureInfo(const APlacedStructure* Structure)
{
	FGridStructureInfo Result{};
	
	FGameplayTag StructureTag = Structure->GetStructureTag();
	EGridBuildingType BuildingType = TagToType[StructureTag];
	
	Result.GridLocation = UFBCBlueprintLibrary::GetGridCoordinateLocation(Structure->GetActorLocation());
	Result.BuildingType = BuildingType;
	return Result;
}

EWallDirection UGridWorldSubsystem::GetWallDirection(const FRotator& WallRotation)
{
	return static_cast<EWallDirection>(UFBCBlueprintLibrary::SnapAngleToGridInt(WallRotation.Yaw) / 90);
}


