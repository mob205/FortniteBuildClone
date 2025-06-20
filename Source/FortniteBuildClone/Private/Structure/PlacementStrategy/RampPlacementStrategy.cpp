// Fill out your copyright notice in the Description page of Project Settings.


#include "Structure/PlacementStrategy/RampPlacementStrategy.h"

#include "FBCBlueprintLibrary.h"
#include "GridSizes.h"

bool URampPlacementStrategy::GetTargetingLocation(
	APawn* Player, int RotationOffset, int32 Edit, FTransform& OutResult)
{
	APlayerController* PC = Cast<APlayerController>(Player->GetController());
	
	FCollisionObjectQueryParams ObjectQueryParams{};
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	FVector TargetLocation = GetViewLocation(PC, ObjectQueryParams);

	// DrawDebugSphere(GetWorld(), TargetLocation, 10.f, 10, FColor::Red);

	OutResult.SetLocation(UFBCBlueprintLibrary::SnapLocationToGrid_FloorZ(TargetLocation));

	const float Yaw = UFBCBlueprintLibrary::SnapAngleToGrid(PC->GetControlRotation().Yaw + (RotationOffset * 90.0f));

	FRotator TargetRotator = {0, Yaw, 0};
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
	OutResult.SetLocation(UFBCBlueprintLibrary::SnapLocationToGrid_FloorZ(TargetLocation));

	if (CanPlace(OutResult, Edit) && !IsOccupied(OutResult))
	{
		return true;
	}

	// Try placing in one below the player's vertical block
	OutResult.SetLocation(UFBCBlueprintLibrary::SnapLocationToGrid_FloorZ(TargetLocation) - FVector{0, 0, GridSizeVertical });
	if (CanPlace(OutResult, Edit) && !IsOccupied(OutResult))
	{
		return true;
	}
	
	// Use cached target location to show invalid placement indicator
	OutResult = PrimaryTargetLocation;
	return false;
}
