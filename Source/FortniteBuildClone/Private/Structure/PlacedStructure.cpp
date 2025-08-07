// Fill out your copyright notice in the Description page of Project Settings.


#include "Structure/PlacedStructure.h"

#include "GameplayEffect.h"
#include "AbilitySystem/FBCAttributeSet.h"
#include "Component/StructureGroundingComponent.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Player/FBCPlayerController.h"

APlacedStructure::APlacedStructure()
{
	PrimaryActorTick.bCanEverTick = false;
	UpdateOverlapsMethodDuringLevelStreaming = EActorUpdateOverlapsMethod::AlwaysUpdate;
	
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	Root->SetMobility(EComponentMobility::Static);
	SetRootComponent(Root);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	StaticMesh->SetupAttachment(Root);

	GroundingComponent = CreateDefaultSubobject<UStructureGroundingComponent>("Grounding Component");
}

void APlacedStructure::PreInitDisableReplication()
{
	bReplicates = false;
}

bool APlacedStructure::IsValidForNeighbor() const
{
	return GroundingComponent->IsValidNeighbor();
}

void APlacedStructure::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams PushParams{};
	PushParams.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS_FAST(APlacedStructure, Health, PushParams)

	DOREPLIFETIME_CONDITION(APlacedStructure, StructureTag, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(APlacedStructure, EditBitfield, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(APlacedStructure, ResourceType, COND_InitialOnly);
}

void APlacedStructure::BeginPlay()
{
	Super::BeginPlay();

	GroundingComponent->OnStructureDisabled.AddDynamic(this, &ThisClass::DisableStructure);
}

void APlacedStructure::DisableStructure()
{
	Root->SetVisibility(false, true);
	SetActorEnableCollision(false);
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

void APlacedStructure::Damage(FGameplayEffectSpecHandle DamageEffectSpec)
{
	check(DamageEffectSpec.IsValid());

	DamageEffectSpec.Data->CalculateModifierMagnitudes();

	for (int i = 0; i < DamageEffectSpec.Data->Modifiers.Num(); ++i)
	{
		if (DamageEffectSpec.Data->Def->Modifiers[i].Attribute == UFBCAttributeSet::GetIncomingDamageAttribute())
		{
			ModifyHealth(-DamageEffectSpec.Data->Modifiers[i].GetEvaluatedMagnitude());
		}
	}
}

void APlacedStructure::OnRep_Health()
{
	if (Health <= 0)
	{
		GroundingComponent->FinishStructureDestruction();
	}
}

void APlacedStructure::SetHealth(float InHealth)
{
	Health = InHealth;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, Health, this);

	OnRep_Health();
}

void APlacedStructure::ModifyHealth(float Amount)
{
	float NewHealth = FMath::Clamp(Health + Amount, 0.f, 100.f);
	SetHealth(NewHealth);
}
