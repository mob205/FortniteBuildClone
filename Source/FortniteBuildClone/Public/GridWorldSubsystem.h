// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GameplayTagContainer.h"
#include "GridWorldSubsystem.generated.h"

class APlacedStructure;

enum class EGridBuildingType : uint8
{
	GRID_Main,
	GRID_Floor,
	GRID_Wall,
	GRID_Max
};

enum class EWallDirection : uint8
{
	DIR_North,
	DIR_East,
	DIR_South,
	DIR_West
};

struct FGridSlotInfo
{
	APlacedStructure* Main{};
	APlacedStructure* Floor{};
	TStaticArray<APlacedStructure*, 4> Walls{};
};

struct FGridStructureInfo
{
	FIntVector GridLocation{};
	EGridBuildingType BuildingType{};
};

/**
 * 
 */
UCLASS()
class FORTNITEBUILDCLONE_API UGridWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UGridWorldSubsystem();
	
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	// Registers the structure into the grid
	void RegisterPlacedStructure(APlacedStructure* Structure);

	// Returns true if the grid space at the specified transform is occupied for the structure
	bool IsOccupied(const FTransform& Transform, FGameplayTag StructureTag);
	
	
private:
	/**
	 * Attempts to find where a structure is stored in the grid
	 *	@param GridPosition The coordinates of the grid slot to check
	 *	@param BuildingType Type of building to look for
	 *	@param WallRotation Rotation of the structure. Only used to distinguish walls
	 *	@param OutStructureRef Out parameter with a reference to the structure pointer
	 *	@returns True if the pointer was successfully found. Returns false if the grid slot doesn't exist.
	 *	Return value does NOT depend on if OutStructureRef is null.
	 */
	APlacedStructure** FindStructureAtPosition(const FIntVector& GridPosition, EGridBuildingType BuildingType,
		const FRotator& WallRotation);

	// Helper function for FindStructureAtPosition
	APlacedStructure** FindMainStructureAtPosition(const FIntVector& GridPosition, EGridBuildingType BuildingType);

	// Helper function for FindStructureAtPosition
	APlacedStructure** FindWallStructureAtPosition(const FIntVector& GridPosition, EWallDirection WallDirection);

	
	TMap<FIntVector, FGridSlotInfo> Grid;
	
	TMap<FGameplayTag, EGridBuildingType> TagToType{};

	FGridStructureInfo GetGridStructureInfo(const APlacedStructure* Structure);

	static EWallDirection GetWallDirection(const FRotator& WallRotation);
};
