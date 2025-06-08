// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FBCCharacter.h"
#include "AbilitySystem/FBCAbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffect.h"
#include "Data/StructureInfoDataAsset.h"
#include "InputAction.h"
#include "FortniteBuildClone/FortniteBuildClone.h"
#include "Player/FBCPlayerState.h"

UAbilitySystemComponent* AFBCCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

// Called on server only
void AFBCCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();
	GrantInitialAbilities();
	InitializeAttributes();
}

// Called on clients only
void AFBCCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilityActorInfo();

	PC = Cast<APlayerController>(GetController());
}

void AFBCCharacter::BeginPlay()
{
	Super::BeginPlay();

	BuildAbilityTag = FGameplayTag::RequestGameplayTag("Abilities.Build");
}

void AFBCCharacter::InitAbilityActorInfo()
{
	AFBCPlayerState* PS = GetPlayerState<AFBCPlayerState>();
	check(PS);

	ASC = PS->GetAbilitySystemComponent();
	ASC->InitAbilityActorInfo(PS, this);

	AS = PS->GetAttributeSet();
}

void AFBCCharacter::GrantInitialAbilities()
{
	for (const auto& Ability : InitialAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec{Ability, 1};
		ASC->GiveAbility(AbilitySpec);
	}
}

void AFBCCharacter::InitializeAttributes()
{
	if (!InitialAttributesEffect) { return; }

	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	FGameplayEffectSpecHandle Effect = ASC->MakeOutgoingSpec(InitialAttributesEffect, 1, Context);
	ASC->ApplyGameplayEffectSpecToSelf(*Effect.Data);
}

void AFBCCharacter::OnBuildAction(UInputAction* InputAction)
{
	// Get structure tag associated with the input
	FGameplayTag StructureTag = StructureInfo->GetTagFromInput(InputAction);

	if (!StructureTag.IsValid())
	{
		UE_LOG(LogFBC, Warning, TEXT("Invalid build action [%s] provided to OnBuildAction."), *InputAction->GetName());
	}

	// Alert the ability of switching structure locally
	HandleBuildAction(StructureTag);

	// Alert server of initially selected structure
	if (GetLocalRole() != ROLE_Authority)
	{
		ServerOnBuildAction(StructureTag);
	}
}

void AFBCCharacter::HandleBuildAction(const FGameplayTag StructureTag) const
{
	ASC->TryActivateAbilitiesByTag(BuildAbilityTag.GetSingleTagContainer());
	FGameplayEventData Payload{};
	ASC->HandleGameplayEvent(StructureTag, &Payload);
}

void AFBCCharacter::ServerOnBuildAction_Implementation(FGameplayTag StructureTag)
{
	FGameplayEventData Payload{};
	ASC->HandleGameplayEvent(StructureTag, &Payload);
}
