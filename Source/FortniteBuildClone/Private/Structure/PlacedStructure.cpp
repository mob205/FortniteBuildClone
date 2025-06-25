// Fill out your copyright notice in the Description page of Project Settings.


#include "Structure/PlacedStructure.h"

#include "FBCBlueprintLibrary.h"
#include "FortniteBuildClone/FortniteBuildClone.h"
#include "Net/UnrealNetwork.h"
#include "Subsystem/DestructionSubsystem.h"

APlacedStructure::APlacedStructure()
{
	PrimaryActorTick.bCanEverTick = false;
	UpdateOverlapsMethodDuringLevelStreaming = EActorUpdateOverlapsMethod::AlwaysUpdate;
	
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	Root->SetMobility(EComponentMobility::Static);
	SetRootComponent(Root);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	StaticMesh->SetupAttachment(Root);
}

void APlacedStructure::NotifyGroundUpdate_Implementation()
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
		DestructionDelay,
		false);
}

void APlacedStructure::RemoveNeighbor(APlacedStructure* Structure, ENeighborRemovalGroundUpdateRule GroundUpdateRule)
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

void APlacedStructure::AddNeighbor(APlacedStructure* Structure)
{
	Neighbors.Add(Structure);
}

void APlacedStructure::ReportNeighbors()
{
	FString Log = FString::Printf(TEXT("Neighbors of %s: "), *GetName());
	for (const auto Neighbor : Neighbors)
	{
		Log += FString::Printf(TEXT("%s, "), *Neighbor->GetName());
	}
	UE_LOG(LogFBC, Warning, TEXT("%s"), *Log);

	TArray<AActor*> Overlaps;
	GetOverlappingActors(Overlaps);
	
	FString Log2 = FString::Printf(TEXT("Overlapping actors of %s: "), *GetName());
	for (const auto Neighbor : Overlaps)
	{
		Log2 += FString::Printf(TEXT("%s, "), *Neighbor->GetName());
	}
	UE_LOG(LogFBC, Warning, TEXT("%s"), *Log2);
}

void APlacedStructure::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APlacedStructure, StructureTag, COND_None);
	DOREPLIFETIME_CONDITION(APlacedStructure, EditBitfield, COND_None);
	DOREPLIFETIME_CONDITION(APlacedStructure, ResourceType, COND_None);
}

void APlacedStructure::BeginPlay()
{
	Super::BeginPlay();

	// Objects with RF_WasLoaded are placed in level in editor and will have their overlaps already updated
	if (!HasAnyFlags(RF_WasLoaded))
	{
		UpdateOverlaps();
	}
		
	InitializeNeighbors();
	
	if (HasAuthority())
	{
		DestructionSubsystem = GetWorld()->GetSubsystem<UDestructionSubsystem>();
	}
}


void APlacedStructure::InitializeNeighbors()
{
	TArray<AActor*> OverlappingActors{};
	GetOverlappingActors(OverlappingActors);
	for (const auto OverlappingActor : OverlappingActors)
	{
		if (APlacedStructure* AsStructure = Cast<APlacedStructure>(OverlappingActor))
		{
			Neighbors.Add(AsStructure);
			AsStructure->AddNeighbor(this);
		}
		else if (UFBCBlueprintLibrary::IsGround(OverlappingActor))
		{
			bIsGroundingStructure = true;
		}
	}
}

void APlacedStructure::FinishStructureDestruction()
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
		GetWorldTimerManager().SetTimer(DelayedDisable, FTimerDelegate::CreateUObject(this, &APlacedStructure::DisableStructure), .01, false);
	}
	if (DestructionSubsystem)
	{
		DestructionSubsystem->QueueDestruction(this);
	}
}

void APlacedStructure::DisableStructure()
{
	{
		TRACE_CPUPROFILER_EVENT_SCOPE_STR("Set Actor Hidden");
		SetActorHiddenInGame(true);
	}
	{
		TRACE_CPUPROFILER_EVENT_SCOPE_STR("Disable Collision");
		SetActorEnableCollision(false);
	}
}

void APlacedStructure::SetStructureMeshVisibility(bool bIsVisible)
{
	StaticMesh->SetVisibility(bIsVisible);
	StaticMesh->SetCollisionResponseToChannel(ECC_Visibility, bIsVisible ? ECR_Block : ECR_Ignore);

	BP_SetStructureVisibility(bIsVisible);
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

void APlacedStructure::OnRep_ResourceType(EFBCResourceType NewResourceType)
{
	UpdateMeshMaterial();
}

void APlacedStructure::SetResourceType(EFBCResourceType InResourceType)
{
	ResourceType = InResourceType;

	UpdateMeshMaterial();
}

void APlacedStructure::UpdateMeshMaterial()
{
	if (MaterialMap.Contains(ResourceType))
	{
		SetStructureMeshMaterial(MaterialMap[ResourceType]);
	}
}

void APlacedStructure::SetStructureMeshMaterial_Implementation(UMaterialInstance* Material)
{
	StaticMesh->SetMaterial(0, Material);
}

bool APlacedStructure::IsGrounded()
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
	
	TSet<APlacedStructure*> SeenStructures{};
	TQueue<APlacedStructure*> Queue{};
	Queue.Enqueue(this);

	while (!Queue.IsEmpty())
	{
		APlacedStructure* CurStructure = *Queue.Peek();
		Queue.Pop();

		TRACE_CPUPROFILER_EVENT_SCOPE_STR("GroundActor");

		const TSet<APlacedStructure*>& CurrentNeighbors = CurStructure->GetNeighbors();
		
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

void APlacedStructure::SetCacheOnStructures(TSet<APlacedStructure*>& Structures, bool bIsGrounded)
{
	TRACE_CPUPROFILER_EVENT_SCOPE_STR("Caching");
	for (const auto& Structure : Structures)
	{
		Structure->SetGroundCache(bIsGrounded);
	}
}
