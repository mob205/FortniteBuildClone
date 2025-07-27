// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/WeaponBase.h"

#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/GameplayTags/FBCTags.h"
#include "Item/Weapon/WeaponItemData.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Player/FBCCharacter.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AWeaponBase::MarkAmmoDirty() const
{
	if (HasAuthority())
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, CurrentAmmo, this);
	}
}

void AWeaponBase::OnItemEquipped(AFBCCharacter* AvatarActor)
{
	Super::OnItemEquipped(AvatarActor);

	SetActorTickEnabled(true);
	OwnerASC = AvatarActor->GetAbilitySystemComponent();
	OwnerASC->GenericGameplayEventCallbacks[FBCTags::InputFireDown]
		.AddUObject(this, &ThisClass::OnFireDown);

	OwnerASC->GenericGameplayEventCallbacks[FBCTags::InputFireReleased]
		.AddUObject(this, &ThisClass::OnFireReleased);
}

void AWeaponBase::OnItemUnequipped(AFBCCharacter* AvatarActor)
{
	Super::OnItemUnequipped(AvatarActor);
	SetActorTickEnabled(false);

	OwnerASC->GenericGameplayEventCallbacks[FBCTags::InputFireDown]
		.RemoveAll(this);

	OwnerASC->GenericGameplayEventCallbacks[FBCTags::InputFireReleased]
		.RemoveAll(this);
}

void AWeaponBase::OnRep_CurrentAmmo()
{
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("Replicated ammo %d"), CurrentAmmo));
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS_FAST(AWeaponBase, CurrentAmmo, Params);
}

void AWeaponBase::OnFireDown(const FGameplayEventData* GameplayEventData)
{
	SetWantsToShoot(true);
}

void AWeaponBase::OnFireReleased(const FGameplayEventData* GameplayEventData)
{
	SetWantsToShoot(false);
}

void AWeaponBase::SetWantsToShoot(bool bNewWantsToShoot)
{
	bWantsToShoot = bNewWantsToShoot;
}

void AWeaponBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (CurrentFireDelay > 0.0f)
	{
		CurrentFireDelay -= DeltaSeconds;
	}
	else if (bWantsToShoot)
	{
		TryWeaponFire();
	}
}

void AWeaponBase::TryWeaponFire()
{
	FGameplayTagContainer Container{};
	Container.AddTag(FBCTags::FireWeapon);
	OwnerASC->TryActivateAbilitiesByTag(Container, true);
}

void AWeaponBase::ResetFireDelay()
{
	CurrentFireDelay = GetWeaponItemData()->GetFireDelay();
}



