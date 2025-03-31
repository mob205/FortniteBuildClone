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
	GRID_NorthWall,
	GRID_EastWall,
	GRID_Max
};

struct FGridStructureInfo
{
	FIntVector GridLocation;
	EGridBuildingType BuildingType;
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
	
	APlacedStructure* GetStructureAtPosition(FIntVector GridPosition, EGridBuildingType BuildingType, bool bIsEastWall = false);
private:
	TMap<FIntVector, TStaticArray<APlacedStructure*, static_cast<int>(EGridBuildingType::GRID_Max)>> Grid;
	
	TMap<FGameplayTag, EGridBuildingType> TagToType{};

	FGridStructureInfo GetGridStructureInfo(APlacedStructure* Structure);
};
