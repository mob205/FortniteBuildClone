// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlacementStrategy.h"
#include "WallPlacementStrategy.generated.h"

/**
 * 
 */
UCLASS()
class FORTNITEBUILDCLONE_API UWallPlacementStrategy : public UPlacementStrategy
{
	GENERATED_BODY()

public: 
	virtual bool GetTargetingLocation(APawn* Player, int RotationOffset, int32 Edit, FTransform& OutResult) override;

protected:
	virtual bool IsStructureOccupying(const FTransform& QueryTransform, const APlacedStructure* Structure) const override;
};
