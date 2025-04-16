// Fill out your copyright notice in the Description page of Project Settings.


#include "Build/PlacedStructure.h"

#include "FBCBlueprintLibrary.h"
#include "Subsystem/GridWorldSubsystem.h"
#include "FortniteBuildClone/FortniteBuildClone.h"
#include "Net/UnrealNetwork.h"

APlacedStructure::APlacedStructure()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APlacedStructure::StartStructureDestruction()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if (TimerManager.TimerExists(DestroyTimerHandle) &&
		TimerManager.GetTimerRemaining(DestroyTimerHandle) < .03) { return; }
	
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
	UE_LOG(LogFBC, Display, TEXT("Destroying structure %s at grid slot %s"), *GetActorNameOrLabel(),
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

void APlacedStructure::SetGroundCache(bool bIsGrounded)
{
	bIsGroundedCached = bIsGrounded;
	GroundCacheTimestamp = GetWorld()->GetTimeSeconds();
}
bool APlacedStructure::IsGroundCacheValid() const
{
	return GroundCacheTimestamp == GetWorld()->GetTimeSeconds();
}

bool APlacedStructure::IsGrounded()
{
	if (IsGroundCacheValid())
	{
		return bIsGroundedCached;
	}
	
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
				if (AsStructure->IsGroundCacheValid())
				{
					bool bIsGrounded = AsStructure->GetGroundCache();
					SetCacheOnStructures(SeenStructures, bIsGrounded);
					return bIsGrounded;
				}
				SeenStructures.Add(AsStructure);
				Queue.Enqueue(AsStructure);
			}
			else if (UFBCBlueprintLibrary::IsGround(Actor))
			{
				SetCacheOnStructures(SeenStructures, true);
				return true;
			}
		}
	}
	SetCacheOnStructures(SeenStructures, false);
	return false;
}

void APlacedStructure::SetCacheOnStructures(TSet<APlacedStructure*> Structures, bool bIsGrounded)
{
	for (const auto& Structure : Structures)
	{
		Structure->SetGroundCache(bIsGrounded);
	}
}

