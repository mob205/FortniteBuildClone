// Fill out your copyright notice in the Description page of Project Settings.


#include "Structure/PlacementStrategy/PlacementStrategy.h"

#include "FBCBlueprintLibrary.h"
#include "Structure/PlacedStructure.h"

bool UPlacementStrategy::CanPlace(const FTransform& QueryTransform) const
{
	OverlapQueryActor->SetActorTransform(QueryTransform);
	OverlapQueryActor->SetActorEnableCollision(true);
	
	TArray<AActor*> OverlappingActors{};
	OverlapQueryActor->GetOverlappingActors(OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (APlacedStructure* AsPlacedStructure = Cast<APlacedStructure>(OverlappingActor))
		{
			OverlapQueryActor->SetActorEnableCollision(false);
			return true;
		}
		if (UFBCBlueprintLibrary::IsGround(OverlappingActor))
		{
			OverlapQueryActor->SetActorEnableCollision(false);
			return true;
		}
	}
	
	OverlapQueryActor->SetActorEnableCollision(false);
	
	return false;
}

void UPlacementStrategy::InitializeStrategy(UGridWorldSubsystem* InGridSubsystem)
{
	GridSubsystem = InGridSubsystem;
	OverlapQueryActor = GetWorld()->SpawnActor(OverlapQueryActorClass);
	OverlapQueryActor->SetActorEnableCollision(false);
}

FVector UPlacementStrategy::GetViewLocation(APlayerController* PC, const FCollisionObjectQueryParams& ObjectQueryParams) const
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
