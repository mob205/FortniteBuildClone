// Fill out your copyright notice in the Description page of Project Settings.


#include "Build/PlacementStrategy/RampPlacementStrategy.h"

#include "FBCBlueprintLibrary.h"

bool URampPlacementStrategy::GetTargetingLocation(
	int RotationOffset, FTransform& OutResult)
{
	FCollisionObjectQueryParams ObjectQueryParams{};
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	FVector TargetLocation = GetViewLocation(ObjectQueryParams);
	
	OutResult.SetLocation(UFBCBlueprintLibrary::SnapLocationToGrid_FloorZ(TargetLocation));

	const float Yaw = UFBCBlueprintLibrary::SnapAngleToGrid(PC->GetControlRotation().Yaw + (RotationOffset * 90.0f));

	FRotator TargetRotator = {0, Yaw, 0};
	OutResult.SetRotation(TargetRotator.Quaternion());

	FTransform PrimaryTargetLocation = OutResult;
	
	if (CanPlace(OutResult))
	{
		return true;
	}

	// Try placing in same vertical block as player
	TargetLocation.Z = Player->GetActorLocation().Z;
	OutResult.SetLocation(UFBCBlueprintLibrary::SnapLocationToGrid_FloorZ(TargetLocation));

	if (CanPlace(OutResult))
	{
		return true;
	}

	// Use cached target location to show invalid placement indicator
	OutResult = PrimaryTargetLocation;
	return false;
}
