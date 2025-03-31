// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlacementStrategy.h"
#include "PyramidPlacementStrategy.generated.h"

/**
 * 
 */
UCLASS()
class FORTNITEBUILDCLONE_API UPyramidPlacementStrategy : public UPlacementStrategy
{
	GENERATED_BODY()

public:
	virtual bool GetTargetingLocation(APlayerController* PlayerController, UGridWorldSubsystem* GridSubsystem, float Range, int RotationOffset, FTransform& OutResult) override;

	virtual bool CanPlace(UGridWorldSubsystem* GridSubsystem, FTransform QueryTransform) override;
};
