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
	virtual bool GetTargetingLocation(APlayerController* PC, UGridWorldSubsystem* GridSubsystem, int RotationOffset, FTransform& OutResult) override;

	virtual bool CanPlace(UGridWorldSubsystem* GridSubsystem, const FTransform& QueryTransform) override;
};
