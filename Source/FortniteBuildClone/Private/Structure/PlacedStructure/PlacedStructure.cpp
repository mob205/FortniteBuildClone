// Fill out your copyright notice in the Description page of Project Settings.


#include "Structure/PlacedStructure/PlacedStructure.h"
#include "AbilitySystem/FBCAttributeSet.h"
#include "Net/UnrealNetwork.h"

APlacedStructure::APlacedStructure()
{
	PrimaryActorTick.bCanEverTick = false;
	UpdateOverlapsMethodDuringLevelStreaming = EActorUpdateOverlapsMethod::AlwaysUpdate;
}

void APlacedStructure::PreInitDisableReplication()
{
	bReplicates = false;
}

void APlacedStructure::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(APlacedStructure, StructureTag, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(APlacedStructure, EditBitfield, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(APlacedStructure, ResourceType, COND_InitialOnly);
}

void APlacedStructure::SetStructureMeshVisibility(bool bIsVisible)
{
	StaticMesh->SetVisibility(bIsVisible);
	StaticMesh->SetCollisionResponseToChannel(ECC_Visibility, bIsVisible ? ECR_Block : ECR_Ignore);

	BP_SetStructureVisibility(bIsVisible);
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

