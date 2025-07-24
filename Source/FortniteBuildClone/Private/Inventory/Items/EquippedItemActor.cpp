// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Items/EquippedItemActor.h"

#include "AbilitySystemComponent.h"
#include "Inventory/ItemData.h"
#include "Net/UnrealNetwork.h"
#include "Player/FBCCharacter.h"

AEquippedItemActor::AEquippedItemActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
}

void AEquippedItemActor::BeginPlay()
{
	Super::BeginPlay();

	SetActorHiddenInGame(true);
}

void AEquippedItemActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AEquippedItemActor, ItemData, COND_InitialOnly);
}

void AEquippedItemActor::OnItemEquipped(AFBCCharacter* AvatarActor)
{
	FAttachmentTransformRules TransformRules = { EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false };
	AttachToComponent(AvatarActor->GetMesh(), TransformRules, ItemData->GetSocketName());

	if (UAnimMontage* EquipMontage = ItemData->GetEquipMontage())
	{
		AvatarActor->PlayAnimMontage(EquipMontage);
	}

	const TArray<TSubclassOf<UGameplayAbility>>& GrantedAbilities = ItemData->GetGrantedAbilities();
	if (HasAuthority() && !GrantedAbilities.IsEmpty())
	{
		UAbilitySystemComponent* ASC = AvatarActor->GetAbilitySystemComponent();
		if (!ASC) { return; }

		for (TSubclassOf<UGameplayAbility> AbilityClass : GrantedAbilities)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec{AbilityClass, 1};
			AbilitySpec.SourceObject = this;
			GrantedAbilityHandles.Add(ASC->GiveAbility(AbilitySpec));
		}
	}
}

void AEquippedItemActor::OnItemUnequipped(AFBCCharacter* AvatarActor)
{
	if (HasAuthority() && !GrantedAbilityHandles.IsEmpty())
	{
		UAbilitySystemComponent* ASC = AvatarActor->GetAbilitySystemComponent();
		if (!ASC) { return; }
		
		for (const FGameplayAbilitySpecHandle Handle : GrantedAbilityHandles)
		{
			ASC->ClearAbility(Handle);
		}
	}
}
