// Fill out your copyright notice in the Description page of Project Settings.


#include "Structure/PlacementStrategy/WallPlacementStrategy.h"
#include "FBCBlueprintLibrary.h"
#include "GridSizes.h"
#include "Structure/PlacedStructure.h"

bool UWallPlacementStrategy::GetTargetingLocation(
	APawn* Player, int RotationOffset, int32 Edit, FTransform& OutResult)
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

bool UWallPlacementStrategy::IsStructureOccupying(const FTransform& QueryTransform,
	const APlacedStructure* Structure) const
{
	int QueryYaw = UFBCBlueprintLibrary::SnapAngleToGridInt(QueryTransform.Rotator().Yaw);
	int StructureYaw = UFBCBlueprintLibrary::SnapAngleToGridInt(Structure->GetActorRotation().Yaw);
	int FlippedQueryYaw = (QueryYaw + 180) % 360;

	FIntVector QueryCoordinateLocation = UFBCBlueprintLibrary::GetGridCoordinateLocation(QueryTransform.GetLocation());
	FIntVector StructureCoordinateLocation = UFBCBlueprintLibrary::GetGridCoordinateLocation(Structure->GetActorLocation());

	bool bIsSameWall = QueryYaw == StructureYaw
						&& QueryCoordinateLocation == StructureCoordinateLocation
						&& IncompatibleStructureTags.HasTagExact(Structure->GetStructureTag());

	// Nearby walls with flipped orientation that we need to consider as "occupying the spot" are either 1 away in the X or 1 away in the Y, but never both
	bool bIsXNeighbor = abs(QueryCoordinateLocation.X - StructureCoordinateLocation.X) == 1;
	bool bIsYNeighbor = abs(QueryCoordinateLocation.Y - StructureCoordinateLocation.Y) == 1;
	
	bool bIsOppositeWall = FlippedQueryYaw == StructureYaw
						&& QueryCoordinateLocation.Z == StructureCoordinateLocation.Z
						&& (bIsXNeighbor != bIsYNeighbor) // XOR
						&& IncompatibleStructureTags.HasTagExact(Structure->GetStructureTag());

	return bIsSameWall || bIsOppositeWall;
}