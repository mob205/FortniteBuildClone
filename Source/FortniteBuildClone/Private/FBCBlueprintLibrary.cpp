// Fill out your copyright notice in the Description page of Project Settings.

#include "FBCBlueprintLibrary.h"

constexpr float GridX{512};
constexpr float GridY{512};
constexpr float GridZ{384};
constexpr float SnapRotation{90};

FTransform UFBCBlueprintLibrary::SnapTransformToGrid(const FTransform& InTransform)
{
	FTransform Result{};
	Result.SetRotation(SnapRotationToGrid(InTransform.Rotator()).Quaternion());
	Result.SetLocation(SnapLocationToGrid(InTransform.GetLocation()));
	return Result;
}

FRotator UFBCBlueprintLibrary::SnapRotationToGrid(const FRotator& InRotator)
{
	FRotator Result{0, 0, 0};
	Result.Yaw = SnapAngleToGrid(InRotator.Yaw);
	return Result;
}

FVector UFBCBlueprintLibrary::SnapLocationToGrid(const FVector& InLocation)
{
	FVector Result;
	Result.X = FMath::RoundToFloat(InLocation.X / GridX) * GridX;
	Result.Y = FMath::RoundToFloat(InLocation.Y / GridY) * GridY;
	Result.Z = FMath::RoundToFloat(InLocation.Z / GridZ) * GridZ;
	return Result;
}

float UFBCBlueprintLibrary::SnapAngleToGrid(const float InAngle)
{
	return FMath::RoundToFloat(InAngle / SnapRotation) * SnapRotation;
}
