// Fill out your copyright notice in the Description page of Project Settings.


#include "Build/PlacementStrategy/WallPlacementStrategy.h"
#include "FBCBlueprintLibrary.h"
#include "GridSizes.h"

bool UWallPlacementStrategy::GetTargetingLocation(
	int RotationOffset, FTransform& OutResult)
{
	FCollisionObjectQueryParams ObjectQueryParams{};
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	FVector TargetLocation = GetViewLocation(ObjectQueryParams);
	
	int Yaw = UFBCBlueprintLibrary::SnapAngleToGridInt(PC->GetControlRotation().Yaw);

	// Walls interpret rotations as flips, rather than 90 degree turns
	// Flip the wall
	if (RotationOffset % 2 == 1)
	{
		// TODO: Simplify angle axis call with simple direction array
		TargetLocation += GridSizeHorizontal * FVector::ForwardVector.RotateAngleAxis(Yaw, FVector::UpVector);
		Yaw += 180;
	}
	
	const FRotator TargetRotator = {0, UFBCBlueprintLibrary::SnapAngleToGrid(Yaw), 0};
	
	OutResult.SetLocation(UFBCBlueprintLibrary::SnapLocationToGrid_FloorZ(TargetLocation));
	OutResult.SetRotation(TargetRotator.Quaternion());
	
	return CanPlace(OutResult);
}
