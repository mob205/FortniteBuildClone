// Fill out your copyright notice in the Description page of Project Settings.

#include "FBCBlueprintLibrary.h"
#include "GridSizes.h"

FTransform UFBCBlueprintLibrary::SnapTransformToGrid(const FTransform& InTransform)
{
	FTransform Result{};
	Result.SetRotation(SnapRotationToGrid(InTransform.Rotator()).Quaternion());
	Result.SetLocation(SnapLocationToGrid_FloorZ(InTransform.GetLocation()));
	return Result;
}

FRotator UFBCBlueprintLibrary::SnapRotationToGrid(const FRotator& InRotator)
{
	FRotator Result{0, 0, 0};
	Result.Yaw = SnapAngleToGrid(InRotator.Yaw);
	return Result;
}

FVector UFBCBlueprintLibrary::SnapLocationToGrid_FloorZ(const FVector& InLocation)
{
	FVector Result;
	Result.X = FMath::RoundToFloat(InLocation.X / GridSizeHorizontal) * GridSizeHorizontal;
	Result.Y = FMath::RoundToFloat(InLocation.Y / GridSizeHorizontal) * GridSizeHorizontal;
	Result.Z = FMath::FloorToFloat(InLocation.Z / GridSizeVertical) * GridSizeVertical;
	return Result;
}

FVector UFBCBlueprintLibrary::SnapLocationToGrid_RoundZ(const FVector& InLocation)
{
	FVector Result;
	Result.X = FMath::RoundToFloat(InLocation.X / GridSizeHorizontal) * GridSizeHorizontal;
	Result.Y = FMath::RoundToFloat(InLocation.Y / GridSizeHorizontal) * GridSizeHorizontal;
	Result.Z = FMath::RoundToFloat(InLocation.Z / GridSizeVertical) * GridSizeVertical;
	return Result;
}

float UFBCBlueprintLibrary::SnapAngleToGrid(const float InAngle)
{
	return FMath::RoundToFloat(InAngle / SnapRotation) * SnapRotation;
}

int UFBCBlueprintLibrary::SnapAngleToGridInt(const float InAngle)
{
	return FMath::RoundToInt(InAngle / SnapRotation) * SnapRotation;
}

FIntVector UFBCBlueprintLibrary::GetGridCoordinateLocation(FVector InWorldLocation)
{
	FIntVector Result;
	Result.X = FMath::RoundToInt(InWorldLocation.X / GridSizeHorizontal);
	Result.Y = FMath::RoundToInt(InWorldLocation.Y / GridSizeHorizontal);
	Result.Z = FMath::RoundToInt(InWorldLocation.Z / GridSizeVertical);
	return Result;
}

bool UFBCBlueprintLibrary::IsGround(AActor* Actor)
{
	UPrimitiveComponent* PrimitiveComp = Actor->GetComponentByClass<UPrimitiveComponent>();
	if (PrimitiveComp && PrimitiveComp->GetCollisionObjectType() == ECC_WorldStatic)
	{
		return true;
	}
	return false;
}

bool UFBCBlueprintLibrary::TraceControllerLook(AController* PC, float Range, FHitResult& OutHit)
{
	FVector ViewStart{};
	FRotator ViewRot{};
	PC->GetPlayerViewPoint(ViewStart, ViewRot);

	FVector ViewEnd = ViewStart + (ViewRot.Vector() * Range);
	
	return PC->GetWorld()->LineTraceSingleByChannel(OutHit, ViewStart, ViewEnd, ECollisionChannel::ECC_Visibility);

}
