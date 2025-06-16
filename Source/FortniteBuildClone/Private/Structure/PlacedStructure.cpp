// Fill out your copyright notice in the Description page of Project Settings.


#include "Structure/PlacedStructure.h"

#include "FBCBlueprintLibrary.h"
#include "FortniteBuildClone/FortniteBuildClone.h"
#include "Net/UnrealNetwork.h"

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

void APlacedStructure::NotifyGroundUpdate()
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
	DOREPLIFETIME_CONDITION(APlacedStructure, EditBitfield, COND_None);
	DOREPLIFETIME_CONDITION(APlacedStructure, ResourceType, COND_None);
	
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
			AsStructure->NotifyGroundUpdate();
		}
	}
	Destroy();
}

void APlacedStructure::SetStructureMeshVisibility(bool bIsVisible)
{
	StaticMesh->SetVisibility(bIsVisible);
	StaticMesh->SetCollisionResponseToChannel(ECC_Visibility, bIsVisible ? ECR_Block : ECR_Ignore);
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
