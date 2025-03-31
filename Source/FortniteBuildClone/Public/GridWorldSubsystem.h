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

	void RegisterPlacedStructure(FIntVector GridPosition, APlacedStructure* Structure);

	bool IsOccupied(FIntVector GridPosition, FGameplayTag StructureTag);
private:
	TMap<EGridBuildingType, TMap<FIntVector, APlacedStructure*>> Grid{};
	
	TMap<FGameplayTag, EGridBuildingType> TagToType{};
};
