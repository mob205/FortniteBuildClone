// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlacementStrategy.h"
#include "RampPlacementStrategy.generated.h"

/**
 * 
 */
UCLASS()
class FORTNITEBUILDCLONE_API URampPlacementStrategy : public UPlacementStrategy
{
	GENERATED_BODY()
public:
	virtual bool GetTargetingLocation(APlayerController* PlayerController, UGridWorldSubsystem* GridSubsystem, float Range, int RotationOffset, FTransform& OutResult) override;

	virtual bool CanPlace(UGridWorldSubsystem* GridSubsystem, FTransform QueryTransform) override;
};
