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
	virtual bool GetTargetingLocation(APawn* PlayerController, UGridWorldSubsystem* GridSubsystem, int RotationOffset, FTransform& OutResult) override;

	virtual bool CanPlace(UGridWorldSubsystem* GridSubsystem, const FTransform& QueryTransform) override;
};
