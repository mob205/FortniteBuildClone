// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "PlacementStrategy.generated.h"

class APlayerController;
class UGridWorldSubsystem;

UCLASS(Blueprintable)
class FORTNITEBUILDCLONE_API UPlacementStrategy : public UObject
{
	GENERATED_BODY()

public:
	/**
	 *	Attempts to find a valid location to place a structure of a certain type.
	 *	@param PC The player controller of the building player. Used to find facing direction, etc.
	 *	@param GridSubsystem The grid subsystem of the current world. Used to query the location of other structures
	 *	@param RotationOffset The number of rotations requested by the building player
	 *	@param OutResult Out parameter to store the valid transform, if found
	 *	@returns true if a valid location was found
	 */
	virtual bool GetTargetingLocation(APlayerController* PC, UGridWorldSubsystem* GridSubsystem,
	                                  int RotationOffset, FTransform& OutResult) { return false; }

	/**
	 * @param GridSubsystem The grid subsystem of the current world. Used to query the location of other structures
	 * @param QueryTransform The transform defining the location and rotation to place this structure at
	 * @return true if the structure can be succesfully placed
	 */
	virtual bool CanPlace(UGridWorldSubsystem* GridSubsystem, const FTransform& QueryTransform) { return false; }

	UPROPERTY(EditDefaultsOnly)
	float TargetingRange{};
};
