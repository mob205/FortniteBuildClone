// Fill out your copyright notice in the Description page of Project Settings.


#include "Structure/PlacementStrategy/WallPlacementStrategy.h"
#include "FBCBlueprintLibrary.h"
#include "GridSizes.h"
#include "Subsystem/GridWorldSubsystem.h"

bool UWallPlacementStrategy::GetTargetingLocation(
	APawn* Player, int RotationOffset, FTransform& OutResult)
{
	APlayerController* PC = Cast<APlayerController>(Player->GetController());

	FCollisionObjectQueryParams ObjectQueryParams{};
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	FVector TargetLocation = GetViewLocation(PC, ObjectQueryParams);
	
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
	
	FTransform PrimaryTargetLocation = OutResult;
	
	if (CanPlace(OutResult))
	{
		return true;
	}

	// Try placing in same vertical block as player
	TargetLocation.Z = Player->GetActorLocation().Z;
	OutResult.SetLocation(UFBCBlueprintLibrary::SnapLocationToGrid_FloorZ(TargetLocation));

	if (CanPlace(OutResult) && !GridSubsystem->IsOccupied(OutResult, StructureTag))
	{
		return true;
	}

	// Try the player's current grid slot
	OutResult.SetLocation(UFBCBlueprintLibrary::SnapLocationToGrid_FloorZ(Player->GetActorLocation()));
	if (CanPlace(OutResult) && !GridSubsystem->IsOccupied(OutResult, StructureTag))
	{
		return true;
	}

	// Use cached target location to show invalid placement indicator
	OutResult = PrimaryTargetLocation;
	return false;
}
