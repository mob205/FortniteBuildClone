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
	
	OutResult.SetLocation(UFBCBlueprintLibrary::SnapLocationToGrid(TargetLocation));


	// Try the calculated yaw as well as the flipped yaw
	for (int i = 0; i < 2; ++i)
	{
		const FRotator TargetRotator = {0, Yaw + (i * 180), 0};
		OutResult.SetRotation(TargetRotator.Quaternion());

		if (CanPlace(OutResult))
		{
			return true;
		}
	}
	return false;
}
