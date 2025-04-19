// Fill out your copyright notice in the Description page of Project Settings.


#include "Structure/PlacementStrategy/RampPlacementStrategy.h"

#include "FBCBlueprintLibrary.h"
#include "Subsystem/GridWorldSubsystem.h"

bool URampPlacementStrategy::GetTargetingLocation(
	APawn* Player, int RotationOffset, FTransform& OutResult)
{
	APlayerController* PC = Cast<APlayerController>(Player->GetController());

	FCollisionObjectQueryParams ObjectQueryParams{};
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	FVector TargetLocation = GetViewLocation(PC, ObjectQueryParams);

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

	if (CanPlace(OutResult) && !GridSubsystem->IsOccupied(OutResult, StructureTag))
	{
		return true;
	}

	// Use cached target location to show invalid placement indicator
	OutResult = PrimaryTargetLocation;
	return false;
}
