// Fill out your copyright notice in the Description page of Project Settings.


#include "Structure/PlacementStrategy/PyramidPlacementStrategy.h"

#include "FBCBlueprintLibrary.h"

bool UPyramidPlacementStrategy::GetTargetingLocation(
	APawn* Player, int RotationOffset, int32 Edit, FTransform& OutResult)
{
	APlayerController* PC = Cast<APlayerController>(Player->GetController());

	FCollisionObjectQueryParams ObjectQueryParams{};
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	FVector TargetLocation = GetViewLocation(PC, ObjectQueryParams);
	
	OutResult.SetLocation(UFBCBlueprintLibrary::SnapLocationToGrid_RoundZ(TargetLocation));
	
	const float Yaw = PC->GetControlRotation().Yaw;
	const FRotator TargetRotator = {0, UFBCBlueprintLibrary::SnapAngleToGrid(Yaw + (RotationOffset * 90)), 0};
	OutResult.SetRotation(TargetRotator.Quaternion());

	FTransform PrimaryTargetLocation = OutResult;
	
	// Don't check occupied for the primary targeting location
	// If this is a valid place but it is occupied, no targeting ghost should be visible
	if (CanPlace(OutResult, Edit))
	{
		return true;
	}

	// Try placing in same vertical block as player
	TargetLocation.Z = Player->GetActorLocation().Z;
	OutResult.SetLocation(UFBCBlueprintLibrary::SnapLocationToGrid_RoundZ(TargetLocation));

	if (CanPlace(OutResult, Edit) && !IsOccupied(OutResult))
	{
		return true;
	}

	// Try the player's current grid slot
	OutResult.SetLocation(UFBCBlueprintLibrary::SnapLocationToGrid_FloorZ(Player->GetActorLocation()));
	if (CanPlace(OutResult, Edit) && !IsOccupied(OutResult))
	{
		return true;
	}

	// Use cached target location to show invalid placement indicator
	OutResult = PrimaryTargetLocation;
	return false;
}
