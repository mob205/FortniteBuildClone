// Fill out your copyright notice in the Description page of Project Settings.


#include "Build/PlacementStrategy/PyramidPlacementStrategy.h"

#include "FBCBlueprintLibrary.h"

bool UPyramidPlacementStrategy::GetTargetingLocation(
	int RotationOffset, FTransform& OutResult)
{
	FCollisionObjectQueryParams ObjectQueryParams{};
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	FVector TargetLocation = GetViewLocation(ObjectQueryParams);
	
	OutResult.SetLocation(UFBCBlueprintLibrary::SnapLocationToGrid_RoundZ(TargetLocation));
	
	const float Yaw = PC->GetControlRotation().Yaw;
	const FRotator TargetRotator = {0, UFBCBlueprintLibrary::SnapAngleToGrid(Yaw + (RotationOffset * 90)), 0};
	OutResult.SetRotation(TargetRotator.Quaternion());
	
	return CanPlace(OutResult);
}