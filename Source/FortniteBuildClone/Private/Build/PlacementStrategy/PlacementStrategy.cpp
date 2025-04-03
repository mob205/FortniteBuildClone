// Fill out your copyright notice in the Description page of Project Settings.


#include "Build/PlacementStrategy/PlacementStrategy.h"
#include "Build/PlacedStructure.h"

bool UPlacementStrategy::CanPlace(const FTransform& QueryTransform) const
{
	OverlapQueryActor->SetActorTransform(QueryTransform);
	
	TArray<AActor*> OverlappingActors{};
	OverlapQueryActor->GetOverlappingActors(OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (OverlappingActor->GetRootComponent()->GetCollisionObjectType() == ECC_WorldStatic)
		{
			// Grounded - remember it somehow?
			return true;
		}
		if (APlacedStructure* AsPlacedStructure = Cast<APlacedStructure>(OverlappingActor))
		{
			// Valid support found - tell supporting neighbors?
			return true;
		}
	}
	
	return false;
}

void UPlacementStrategy::InitializeStrategy(APawn* InPlayer, UGridWorldSubsystem* InGridSubsystem)
{
	Player = InPlayer;
	GridSubsystem = InGridSubsystem;

	OverlapQueryActor = InPlayer->GetWorld()->SpawnActor(OverlapQueryActorClass);
}
