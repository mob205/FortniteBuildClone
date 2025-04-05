// Fill out your copyright notice in the Description page of Project Settings.


#include "Build/PlacementStrategy/PlacementStrategy.h"
#include "Build/PlacedStructure.h"

bool UPlacementStrategy::CanPlace(const FTransform& QueryTransform) const
{
	OverlapQueryActor->SetActorTransform(QueryTransform);
	OverlapQueryActor->SetActorEnableCollision(true);
	
	TArray<AActor*> OverlappingActors{};
	OverlapQueryActor->GetOverlappingActors(OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor->GetRootComponent()->GetCollisionObjectType() == ECC_WorldStatic)
		{
			// Grounded - remember it somehow?
			OverlapQueryActor->SetActorEnableCollision(false);
			return true;
		}
		if (APlacedStructure* AsPlacedStructure = Cast<APlacedStructure>(OverlappingActor))
		{
			// Valid support found - tell supporting neighbors?
			OverlapQueryActor->SetActorEnableCollision(false);
			return true;
		}
	}
	
	OverlapQueryActor->SetActorEnableCollision(false);
	
	return false;
}

void UPlacementStrategy::InitializeStrategy(APawn* InPlayer, UGridWorldSubsystem* InGridSubsystem)
{
	Player = InPlayer;
	GridSubsystem = InGridSubsystem;
	PC = Player->GetLocalViewingPlayerController();

	OverlapQueryActor = InPlayer->GetWorld()->SpawnActor(OverlapQueryActorClass);
}

FVector UPlacementStrategy::GetViewLocation(const FCollisionObjectQueryParams& ObjectQueryParams) const
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
