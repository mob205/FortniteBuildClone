// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/StructureGroundingComponent.h"

#include "FBCBlueprintLibrary.h"
#include "Structure/PlacedStructure.h"


UStructureGroundingComponent::UStructureGroundingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UStructureGroundingComponent::DestroyOnClients_Implementation()
{
	FinishStructureDestruction();
}

void UStructureGroundingComponent::BeginPlay()
{
	Super::BeginPlay();

	Owner = Cast<APlacedStructure>(GetOwner());
	
	// Objects with RF_WasLoaded are placed in level in editor and will have their overlaps already updated
	if (!Owner->HasAnyFlags(RF_WasLoaded))
	{
		Owner->UpdateOverlaps();
	}
		
	InitializeNeighbors();
	
	if (Owner->HasAuthority())
	{
		DestructionSubsystem = GetWorld()->GetSubsystem<UDestructionSubsystem>();
	}
}

void UStructureGroundingComponent::InitializeNeighbors()
{
	TArray<AActor*> OverlappingActors{};
	Owner->GetOverlappingActors(OverlappingActors);
	for (const auto OverlappingActor : OverlappingActors)
	{
		if (APlacedStructure* AsStructure = Cast<APlacedStructure>(OverlappingActor))
		{
			UStructureGroundingComponent* GroundingComp = AsStructure->GetGroundingComponent();
			Neighbors.Add(GroundingComp);
			GroundingComp->AddNeighbor(this);
		}
		else if (UFBCBlueprintLibrary::IsGround(OverlappingActor))
		{
			bIsGroundingStructure = true;
		}
	}
}


void UStructureGroundingComponent::RemoveNeighbor(UStructureGroundingComponent* Structure, ENeighborRemovalGroundUpdateRule GroundUpdateRule)
{
	Neighbors.Remove(Structure);
	switch (GroundUpdateRule)
	{
	case NRG_Local:
		NotifyGroundUpdate_Implementation();
		break;
	case NRG_Multicast:
		NotifyGroundUpdate();
		break;
	default:
		break;
	}
}

void UStructureGroundingComponent::NotifyGroundUpdate_Implementation()
{
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if (TimerManager.TimerExists(GroundCheckTimerHandle) &&
		TimerManager.GetTimerRemaining(GroundCheckTimerHandle) < .03) { return; }
	
	GetWorld()->GetTimerManager().SetTimer(
		GroundCheckTimerHandle,
		FTimerDelegate::CreateLambda(
		[this]()
		{
			if (!IsGrounded())
			{
				FinishStructureDestruction();
			}
		}),
		GroundCheckDelay,
		false);
}

void UStructureGroundingComponent::AddNeighbor(UStructureGroundingComponent* Structure)
{
	Neighbors.Add(Structure);
}

void UStructureGroundingComponent::FinishStructureDestruction()
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("FinishDestruction");

	{
		TRACE_CPUPROFILER_EVENT_SCOPE_STR("Neighbors");
		for (auto Neighbor: Neighbors)
		{
			Neighbor->RemoveNeighbor(this, NRG_Local);
		}
	}

	{
		TRACE_CPUPROFILER_EVENT_SCOPE_STR("Disable");

		// Disabling structures is comparable to the grounding check itself
		// Defer this to next frame for smoother performance
		FTimerHandle DelayedDisable{};
		GetWorld()->GetTimerManager().SetTimer(DelayedDisable, FTimerDelegate::CreateUObject(Owner, &APlacedStructure::DisableStructure), .01, false);
	}
	if (DestructionSubsystem)
	{
		DestructionSubsystem->QueueDestruction(Owner);
	}
}

bool UStructureGroundingComponent::IsGrounded()
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("Ground");

	if (bIsGroundingStructure)
	{
		return true;
	}
	if (IsGroundCacheValid())
	{
		return bIsGroundedCached;
	}
	
	TSet<UStructureGroundingComponent*> SeenStructures{};
	TQueue<UStructureGroundingComponent*> Queue{};
	Queue.Enqueue(this);

	while (!Queue.IsEmpty())
	{
		UStructureGroundingComponent* CurStructure = *Queue.Peek();
		Queue.Pop();

		TRACE_CPUPROFILER_EVENT_SCOPE_STR("GroundActor");

		const TSet<UStructureGroundingComponent*>& CurrentNeighbors = CurStructure->GetNeighbors();
		
		for (const auto Neighbor : CurrentNeighbors)
		{
			if (SeenStructures.Contains(Neighbor))
			{
				continue;
			}
			
			if (Neighbor->bIsGroundingStructure)
			{
				SetCacheOnStructures(SeenStructures, true);
				return true;
			}
			if (Neighbor->IsGroundCacheValid())
			{
				bool bIsGrounded = Neighbor->GetGroundCache();
				SetCacheOnStructures(SeenStructures, bIsGrounded);
				return bIsGrounded;
			}

			SeenStructures.Add(Neighbor);
			Queue.Enqueue(Neighbor);
		}
	}
	SetCacheOnStructures(SeenStructures, false);
	return false;
}

void UStructureGroundingComponent::SetGroundCache(bool bIsGrounded)
{
	bIsGroundedCached = bIsGrounded;
	GroundCacheTimestamp = GetWorld()->GetTimeSeconds();
}
void UStructureGroundingComponent::SetCacheOnStructures(TSet<UStructureGroundingComponent*>& Structures, bool bIsGrounded)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("Caching");
	for (const auto& Structure : Structures)
	{
		Structure->SetGroundCache(bIsGrounded);
	}
}
bool UStructureGroundingComponent::IsGroundCacheValid() const
{
	return GroundCacheTimestamp == GetWorld()->GetTimeSeconds();
}




