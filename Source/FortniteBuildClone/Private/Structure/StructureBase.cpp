// Fill out your copyright notice in the Description page of Project Settings.


#include "Structure/StructureBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffect.h"
#include "AbilitySystem/FBCAttributeSet.h"
#include "AbilitySystem/GameplayTags/FBCTags.h"
#include "Component/StructureGroundingComponent.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

AStructureBase::AStructureBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	Root->SetMobility(EComponentMobility::Static);
	SetRootComponent(Root);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("Static Mesh");
	StaticMesh->SetupAttachment(Root);

	GroundingComponent = CreateDefaultSubobject<UStructureGroundingComponent>("Grounding Component");
}

void AStructureBase::OnRep_Health(float OldHealth)
{
	if (Health <= 0)
	{
		GroundingComponent->FinishStructureDestruction();
	}
	else if (Health < OldHealth)
	{
		OnDamageTaken.Broadcast(Health);
	}
}

void AStructureBase::BeginPlay()
{
	Super::BeginPlay();
	GroundingComponent->OnStructureDisabled.AddDynamic(this, &ThisClass::DisableStructure);

	SetHealth(MaxHealth);
}

void AStructureBase::DisableStructure()
{
	Root->SetVisibility(false, true);
	SetActorEnableCollision(false);
}


void AStructureBase::SetHealth(float InHealth)
{
	const float OldHealth = Health;
	Health = InHealth;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, Health, this);

	OnRep_Health(OldHealth);
}

void AStructureBase::ModifyHealth(float Amount)
{
	float NewHealth = FMath::Clamp(Health + Amount, 0.f, 100.f);
	SetHealth(NewHealth);
}

void AStructureBase::Damage(FGameplayEffectSpecHandle DamageEffectSpec)
{
	check(DamageEffectSpec.IsValid());

	DamageEffectSpec.Data->CalculateModifierMagnitudes();

	for (int i = 0; i < DamageEffectSpec.Data->Modifiers.Num(); ++i)
	{
		if (DamageEffectSpec.Data->Def->Modifiers[i].Attribute == UFBCAttributeSet::GetIncomingDamageAttribute())
		{
			ModifyHealth(-DamageEffectSpec.Data->Modifiers[i].GetEvaluatedMagnitude());

			if (DamageEffectSpec.Data->GetDynamicAssetTags().HasTagExact(FBCTags::Harvesting))
			{
				ApplyHarvest(DamageEffectSpec.Data->GetEffectContext().GetInstigator());
			}
			return;
		}
	}
}

void AStructureBase::ApplyHarvest(AActor* Harvester)
{
	UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Harvester);
	if (!ASC) { return; }

	for (const auto& Info : HarvestInfo)
	{
		if (!Info.HarvestEffectClass) { continue; }
		
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(Info.HarvestEffectClass, 1, Context);

		float HarvestMagnitude = FMath::RandRange(Info.MinMagnitude, Info.MaxMagnitude);
		Spec.Data->SetSetByCallerMagnitude(FBCTags::Harvesting, HarvestMagnitude);

		ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data);
	}
}

void AStructureBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams PushParams{};
	PushParams.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Health, PushParams)
}
