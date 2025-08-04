// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FBCCharacterBase.h"
#include "AbilitySystem/FBCAbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffect.h"
#include "Structure/Data/StructureInfoDataAsset.h"
#include "InputAction.h"
#include "AbilitySystem/GameplayTags/FBCTags.h"
#include "Component/InventoryComponent.h"
#include "Component/LagCompensationComponent.h"
#include "Components/BoxComponent.h"
#include "FortniteBuildClone/FortniteBuildClone.h"
#include "Net/UnrealNetwork.h"

AFBCCharacterBase::AFBCCharacterBase()
{
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");

#if WITH_EDITOR || UE_SERVER
	LagCompensationComponent = CreateDefaultSubobject<ULagCompensationComponent>("LagCompensation");
#endif
}

UAbilitySystemComponent* AFBCCharacterBase::GetAbilitySystemComponent() const
{
	return ASC;
}

FCollisionQueryParams AFBCCharacterBase::GetIgnoreCharacterParams() const
{
	FCollisionQueryParams Params{};

	Params.AddIgnoredActor(this);

	return Params;
}

// Called on server only
void AFBCCharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();
	GrantInitialAbilities();
	InitializeAttributes();
	AddInitialEffects();

	CacheHitboxes();
}

void AFBCCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AFBCCharacterBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AFBCCharacterBase, bIsSliding, COND_None, REPNOTIFY_Always);
}

void AFBCCharacterBase::InitAbilityActorInfo()
{
	OnASCInit.Broadcast(ASC);

	ASC->AbilityFailedCallbacks.AddUObject(this, &AFBCCharacterBase::OnAbilityFailed);
}

void AFBCCharacterBase::OnAbilityFailed(const UGameplayAbility* GameplayAbility, const FGameplayTagContainer& GameplayTags)
{
	
}

void AFBCCharacterBase::GrantInitialAbilities()
{
	for (const auto& Ability : InitialAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec{Ability.Ability, 1};
		ASC->GiveAbility(AbilitySpec);

		if (Ability.bActivateImmediately)
		{
			ASC->TryActivateAbility(AbilitySpec.Handle);
		}
	}
}

void AFBCCharacterBase::InitializeAttributes()
{
	if (!InitialAttributesEffect) { return; }

	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	FGameplayEffectSpecHandle Effect = ASC->MakeOutgoingSpec(InitialAttributesEffect, 1, Context);
	ASC->ApplyGameplayEffectSpecToSelf(*Effect.Data);
}

void AFBCCharacterBase::AddInitialEffects()
{
	for (const auto& Effect : InitialEffects)
	{
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(Effect, 1, Context);
		ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data);
	}
}

void AFBCCharacterBase::OnBuildAction(UInputAction* InputAction)
{
	// Get structure tag associated with the input
	FGameplayTag StructureTag = StructureInfo->GetTagFromInput(InputAction);

	if (!StructureTag.IsValid())
	{
		UE_LOG(LogFBC, Warning, TEXT("Invalid build action [%s] provided to OnBuildAction."), *InputAction->GetName());
	}

	// Alert the ability of switching structure locally
	HandleBuildAction(StructureTag);
}

void AFBCCharacterBase::HandleBuildAction(const FGameplayTag StructureTag) const
{
	ASC->TryActivateAbilitiesByTag(FBCTags::Build.GetTag().GetSingleTagContainer());
	FGameplayEventData Payload{};
	ASC->HandleGameplayEvent(StructureTag, &Payload);
}

void AFBCCharacterBase::CacheHitboxes()
{
	TArray<USceneComponent*> ChildHitboxes{};
	GetMesh()->GetChildrenComponents(false, ChildHitboxes);

	for (const auto Child : ChildHitboxes)
	{
		if (UBoxComponent* Hitbox = Cast<UBoxComponent>(Child))
		{
			Hitboxes.Add(Hitbox);
		}
	}
}
