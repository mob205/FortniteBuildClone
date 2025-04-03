// Fill out your copyright notice in the Description page of Project Settings.


#include "Build/PlacementStrategy/FloorPlacementStrategy.h"

#include "FBCBlueprintLibrary.h"

bool UFloorPlacementStrategy::GetTargetingLocation(
	int RotationOffset, FTransform& OutResult)
{
	APlayerController* PC = Player->GetLocalViewingPlayerController();
	
	FVector ViewStart{};
	FRotator ViewRot{};

	PC->GetPlayerViewPoint(ViewStart, ViewRot);

	const FVector ViewDir = ViewRot.Vector();
	const FVector ViewEnd = ViewStart + (ViewDir * TargetingRange);

	FVector TargetLocation{};
	
	FHitResult HitResult{};
	if (GetWorld()->LineTraceSingleByChannel(HitResult, ViewStart, ViewEnd, ECollisionChannel::ECC_Visibility))
	{
		TargetLocation = HitResult.Location;
	}
	else
	{
		TargetLocation = ViewEnd;
	}
	
	OutResult.SetLocation(UFBCBlueprintLibrary::SnapLocationToGrid(TargetLocation));
	
	const float Yaw = PC->GetControlRotation().Yaw;
	const FRotator TargetRotator = {0, UFBCBlueprintLibrary::SnapAngleToGrid(Yaw + (RotationOffset * 90)), 0};
	OutResult.SetRotation(TargetRotator.Quaternion());

	return CanPlace(OutResult);
}
