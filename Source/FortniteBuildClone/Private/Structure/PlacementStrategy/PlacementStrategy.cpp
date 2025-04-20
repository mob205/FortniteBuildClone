// Fill out your copyright notice in the Description page of Project Settings.


#include "Structure/PlacementStrategy/PlacementStrategy.h"

#include "FBCBlueprintLibrary.h"
#include "Structure/PlacedStructure.h"

bool UPlacementStrategy::CanPlace(const FTransform& QueryTransform) const
{
	TArray<AActor*> OverlappingActors{};
	GetNearbyActors(QueryTransform, OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (APlacedStructure* AsPlacedStructure = Cast<APlacedStructure>(OverlappingActor))
		{
			return true;
		}
		if (UFBCBlueprintLibrary::IsGround(OverlappingActor))
		{
			return true;
		}
	}
	return false;
}

bool UPlacementStrategy::IsOccupied(const FTransform& QueryTransform) const
{
	FTransform SnappedTransform = UFBCBlueprintLibrary::SnapTransformToGrid(QueryTransform);
	TArray<AActor*> OverlappingActors{};
	GetNearbyActors(SnappedTransform, OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (APlacedStructure* AsPlacedStructure = Cast<APlacedStructure>(OverlappingActor))
		{
			if (IsStructureOccupying(SnappedTransform, AsPlacedStructure))
			{
				return true;
			}
		}
	}
	return false;
}

bool UPlacementStrategy::IsStructureOccupying(const FTransform& QueryTransform, const APlacedStructure* Structure) const
{
	return IncompatibleStructureTags.HasTagExact(Structure->GetStructureTag()) &&
		UFBCBlueprintLibrary::GetGridCoordinateLocation(QueryTransform.GetLocation()) == UFBCBlueprintLibrary::GetGridCoordinateLocation(Structure->GetActorLocation());
}

void UPlacementStrategy::InitializeStrategy()
{
	OverlapQueryActor = GetWorld()->SpawnActor(OverlapQueryActorClass);
	OverlapQueryActor->SetActorEnableCollision(false);
}

FVector UPlacementStrategy::GetViewLocation(const APlayerController* PC, const FCollisionObjectQueryParams& ObjectQueryParams) const
{
	FVector ViewStart{};
	FRotator ViewRot{};

	PC->GetPlayerViewPoint(ViewStart, ViewRot);

	const FVector ViewDir = ViewRot.Vector();
	const FVector ViewEnd = ViewStart + (ViewDir * TargetingRange);

	FHitResult HitResult{};
	if (GetWorld()->LineTraceSingleByObjectType(HitResult, ViewStart, ViewEnd, ObjectQueryParams))
	{
		return HitResult.Location;
	}
	else
	{
		return ViewEnd;
	}
}

void UPlacementStrategy::GetNearbyActors(const FTransform& QueryTransform, TArray<AActor*>& OutActors) const
{
	FTransform SnappedTransform = UFBCBlueprintLibrary::SnapTransformToGrid(QueryTransform);
	OverlapQueryActor->SetActorTransform(SnappedTransform);
	
	OverlapQueryActor->SetActorEnableCollision(true);
	OverlapQueryActor->GetOverlappingActors(OutActors);
	OverlapQueryActor->SetActorEnableCollision(false);
}
