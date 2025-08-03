// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/General/EquippedItemActor.h"

#include "AbilitySystemComponent.h"
#include "Item/General/ItemData.h"
#include "Net/UnrealNetwork.h"
#include "Player/FBCCharacterBase.h"

AEquippedItemActor::AEquippedItemActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bNetUseOwnerRelevancy = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(RootComp);

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMeshComp->SetupAttachment(RootComp);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

void AEquippedItemActor::OnItemEquipped(AFBCCharacterBase* ItemOwner)
{
	FAttachmentTransformRules TransformRules = { EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false };
	AttachToComponent(ItemOwner->GetMesh(), TransformRules, ItemData->GetSocketName());

	if (UAnimMontage* EquipMontage = ItemData->GetEquipMontage())
	{
		ItemOwner->PlayAnimMontage(EquipMontage);
	}

	const TArray<TSubclassOf<UGameplayAbility>>& GrantedAbilities = ItemData->GetGrantedAbilities();
	if (HasAuthority() && !GrantedAbilities.IsEmpty())
	{
		UAbilitySystemComponent* ASC = ItemOwner->GetAbilitySystemComponent();
		if (!ASC) { return; }

		for (TSubclassOf<UGameplayAbility> AbilityClass : GrantedAbilities)
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec{AbilityClass, 1};
			AbilitySpec.SourceObject = this;
			GrantedAbilityHandles.Add(ASC->GiveAbility(AbilitySpec));
		}
	}
}

void AEquippedItemActor::OnItemUnequipped(AFBCCharacterBase* ItemOwner)
{
	if (HasAuthority() && !GrantedAbilityHandles.IsEmpty())
	{
		UAbilitySystemComponent* ASC = ItemOwner->GetAbilitySystemComponent();
		if (!ASC) { return; }
		
		for (const FGameplayAbilitySpecHandle Handle : GrantedAbilityHandles)
		{
			ASC->ClearAbility(Handle);
		}
	}
}
