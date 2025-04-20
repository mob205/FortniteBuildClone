// Fill out your copyright notice in the Description page of Project Settings.


#include "Structure/PlacedStructure.h"

#include "FBCBlueprintLibrary.h"
#include "Components/SplineComponent.h"
#include "FortniteBuildClone/FortniteBuildClone.h"
#include "Net/UnrealNetwork.h"

APlacedStructure::APlacedStructure()
{
	PrimaryActorTick.bCanEverTick = false;

	Root = CreateDefaultSubobject<USceneComponent>("Root");
	Root->SetMobility(EComponentMobility::Static);
	SetRootComponent(Root);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	StaticMesh->SetupAttachment(Root);
	
	ForwardLedge = CreateDefaultSubobject<USplineComponent>("Forward Ledge");
	ForwardLedge->SetupAttachment(GetRootComponent());

	BackLedge = CreateDefaultSubobject<USplineComponent>("Back Ledge");
	BackLedge->SetupAttachment(GetRootComponent());
	
	LeftLedge = CreateDefaultSubobject<USplineComponent>("Left Ledge");
	LeftLedge->SetupAttachment(GetRootComponent());

	RightLedge = CreateDefaultSubobject<USplineComponent>("Right Ledge");
	RightLedge->SetupAttachment(GetRootComponent());
	
	Ledges.Add(ForwardLedge);
	Ledges.Add(BackLedge);
	Ledges.Add(LeftLedge);
	Ledges.Add(RightLedge);

	OppositeLedges.Add(ForwardLedge, BackLedge);
	OppositeLedges.Add(BackLedge, ForwardLedge);
	OppositeLedges.Add(RightLedge, LeftLedge);
	OppositeLedges.Add(ForwardLedge, BackLedge);
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
	
	//GridWorldSubsystem->UnregisterStructure(this);
	Destroy();
}

void APlacedStructure::SetStructureMeshVisibility(bool bIsVisible)
{
	StaticMesh->SetVisibility(bIsVisible);
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

