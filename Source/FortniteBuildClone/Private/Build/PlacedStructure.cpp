// Fill out your copyright notice in the Description page of Project Settings.


#include "Build/PlacedStructure.h"

#include "FBCBlueprintLibrary.h"
#include "GridWorldSubsystem.h"
#include "FortniteBuildClone/FortniteBuildClone.h"
#include "Net/UnrealNetwork.h"

APlacedStructure::APlacedStructure()
{
	PrimaryActorTick.bCanEverTick = true;
}

void APlacedStructure::StartStructureDestruction()
{
	FTimerHandle DestroyTimerHandle;
	
	GetWorld()->GetTimerManager().SetTimer(
		DestroyTimerHandle,
		FTimerDelegate::CreateLambda(
		[this]()
		{
			if (!IsGrounded())
			{
				FinishStructureDestruction();
			}
		}),
		DestructionDelay,
		false);
}

void APlacedStructure::BeginPlay()
{
	Super::BeginPlay();

	GridWorldSubsystem = GetWorld()->GetSubsystem<UGridWorldSubsystem>();
	GridWorldSubsystem->RegisterPlacedStructure(this);
}

void APlacedStructure::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APlacedStructure, StructureTag, COND_None);

}

void APlacedStructure::FinishStructureDestruction()
{
	UE_LOG(LogFBC, Display, TEXT("Destroying structure %s at grid slot %s"), *GetName(),
		*UFBCBlueprintLibrary::GetGridCoordinateLocation(GetActorLocation()).ToString());

	TArray<AActor*> OverlappingActors{};
	GetOverlappingActors(OverlappingActors);
	for (const auto& Actor : OverlappingActors)
	{
		if (APlacedStructure* AsStructure = Cast<APlacedStructure>(Actor))
		{
			AsStructure->StartStructureDestruction();
		}
	}
	
	GridWorldSubsystem->UnregisterStructure(this);
	Destroy();
}

bool APlacedStructure::IsGrounded()
{
	TSet<APlacedStructure*> SeenStructures{};
	TArray<AActor*> OverlappingActors{};
	TQueue<APlacedStructure*> Queue{};
	Queue.Enqueue(this);

	while (!Queue.IsEmpty())
	{
		APlacedStructure* CurStructure = *Queue.Peek();
		Queue.Pop();

		CurStructure->GetOverlappingActors(OverlappingActors);
		for (const auto& Actor : OverlappingActors)
		{
			if (APlacedStructure* AsStructure = Cast<APlacedStructure>(Actor))
			{
				if (SeenStructures.Contains(AsStructure)) { continue; }
				SeenStructures.Add(AsStructure);
				Queue.Enqueue(AsStructure);
			}
			else if (UFBCBlueprintLibrary::IsGround(Actor))
			{
				return true;
			}
		}
	}
	return false;
}

