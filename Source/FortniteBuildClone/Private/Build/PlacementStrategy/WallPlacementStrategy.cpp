// Fill out your copyright notice in the Description page of Project Settings.


#include "Build/PlacementStrategy/WallPlacementStrategy.h"

#include "FBCBlueprintLibrary.h"
#include "GridSizes.h"

bool UWallPlacementStrategy::GetTargetingLocation(APlayerController* PC,
                                                  UGridWorldSubsystem* GridSubsystem, int RotationOffset, FTransform& OutResult)
{
	FVector ViewStart{};
	FRotator ViewRot{};

	PC->GetPlayerViewPoint(ViewStart, ViewRot);

	const FVector ViewDir = ViewRot.Vector();
	const FVector ViewEnd = ViewStart + (ViewDir * TargetingRange);

	FVector TargetLocation{};
	
	FHitResult HitResult{};
	FCollisionObjectQueryParams ObjectQueryParams{};
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldStatic);
	
	DrawDebugLine(PC->GetWorld(), ViewStart, ViewEnd, FColor::Red, false, 0, 0, 2);
	DrawDebugSphere(PC->GetWorld(), ViewEnd, 5, 10, FColor::Blue, false, 0, 0, 2);
	
	if (GetWorld()->LineTraceSingleByObjectType(HitResult, ViewStart, ViewEnd, ObjectQueryParams))
	{
		TargetLocation = HitResult.Location;
	}
	else
	{
		TargetLocation = ViewEnd;
	}
	
	int Yaw = UFBCBlueprintLibrary::SnapAngleToGridInt(PC->GetControlRotation().Yaw);

	// Walls interpret rotations as flips, rather than 90 degree turns
	// Flip the wall
	if (RotationOffset % 2 == 1)
	{
		TargetLocation += GridSizeHorizontal * FVector::ForwardVector.RotateAngleAxis(Yaw, FVector::UpVector);
		Yaw += 180;
	}
	
	const FRotator TargetRotator = {0, UFBCBlueprintLibrary::SnapAngleToGrid(Yaw), 0};
	
	OutResult.SetLocation(UFBCBlueprintLibrary::SnapLocationToGrid(TargetLocation));
	OutResult.SetRotation(TargetRotator.Quaternion());
	return true;
}

bool UWallPlacementStrategy::CanPlace(UGridWorldSubsystem* GridSubsystem, const FTransform& QueryTransform)
{
	return Super::CanPlace(GridSubsystem, QueryTransform);
}
