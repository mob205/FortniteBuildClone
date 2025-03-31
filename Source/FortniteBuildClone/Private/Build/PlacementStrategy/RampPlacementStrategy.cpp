// Fill out your copyright notice in the Description page of Project Settings.


#include "Build/PlacementStrategy/RampPlacementStrategy.h"

#include "FBCBlueprintLibrary.h"

bool URampPlacementStrategy::GetTargetingLocation(APlayerController* PC,
                                                  UGridWorldSubsystem* GridSubsystem, float Range, int RotationOffset, FTransform& OutResult)
{
	FVector ViewStart{};
	FRotator ViewRot{};

	PC->GetPlayerViewPoint(ViewStart, ViewRot);

	const FVector ViewDir = ViewRot.Vector();
	const FVector ViewEnd = ViewStart + (ViewDir * Range);

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
	const FRotator TargetRotator = {0, UFBCBlueprintLibrary::SnapAngleToGrid(Yaw + 180 + (RotationOffset * 90)), 0};
	OutResult.SetRotation(TargetRotator.Quaternion());
	return true;
}

bool URampPlacementStrategy::CanPlace(UGridWorldSubsystem* GridSubsystem, FTransform QueryTransform)
{
	return Super::CanPlace(GridSubsystem, QueryTransform);
}
