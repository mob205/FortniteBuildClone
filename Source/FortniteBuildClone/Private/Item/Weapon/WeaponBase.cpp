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
	Owner = AvatarActor;
	
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

	if (OwnerASC)
	{
		OwnerASC->GenericGameplayEventCallbacks[FBCTags::InputFireDown].RemoveAll(this);
		OwnerASC->GenericGameplayEventCallbacks[FBCTags::InputFireReleased].RemoveAll(this);
	}
}

void AWeaponBase::OnRep_CurrentAmmo()
{
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("Replicated ammo %d"), CurrentAmmo));
}

void AWeaponBase::OnRep_SpreadSeed()
{
	SpreadStream.Initialize(SpreadSeed);
}

void AWeaponBase::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS_FAST(AWeaponBase, CurrentAmmo, Params);

	DOREPLIFETIME_CONDITION(AWeaponBase, SpreadSeed, COND_InitialOnly);
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

void AWeaponBase::HandleFireSpreadIncrease()
{
	const FSpreadSettings& SpreadSettings = WeaponItemData->GetSpreadSettings();
	float IncreasePerShot = SpreadSettings.IncreasePerShot;
	float MaxAngle = SpreadSettings.MaxAngle;
	
	if (OwnerASC->HasMatchingGameplayTag(FBCTags::Crouching))
	{
		IncreasePerShot *= SpreadSettings.CrouchMultiplier;
		MaxAngle *= SpreadSettings.CrouchMultiplier;
	}
	CurrentWeaponSpread = FMath::Clamp(CurrentWeaponSpread + IncreasePerShot, 0, MaxAngle);
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	SpreadSeed = FMath::Rand32();
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

	UpdateSpread(DeltaSeconds);
}

void AWeaponBase::SetItemData(const UItemData* NewItemData)
{
	Super::SetItemData(NewItemData);
	WeaponItemData = Cast<UWeaponItemData>(NewItemData);
}

void AWeaponBase::TryWeaponFire()
{
	FGameplayTagContainer Container{};
	Container.AddTag(FBCTags::FireWeapon);
	OwnerASC->TryActivateAbilitiesByTag(Container, true);
}

void AWeaponBase::UpdateSpread(float DeltaTime)
{
	const FSpreadSettings& SpreadSettings = WeaponItemData->GetSpreadSettings();

	float TargetSpread = SpreadSettings.BaseAngle;
	float RecoveryRate = SpreadSettings.RecoveryRate;

	if (Owner->GetVelocity().Size() > 0)
	{
		TargetSpread += SpreadSettings.MovementPenalty;
	}

	// TODO: Subscribe to change delegates rather than checking each time
	if (OwnerASC->HasMatchingGameplayTag(FBCTags::Airborne))
	{
		TargetSpread *= SpreadSettings.AirborneMultiplier;
	}
	if (OwnerASC->HasMatchingGameplayTag(FBCTags::Crouching))
	{
		TargetSpread *= SpreadSettings.CrouchMultiplier;
		RecoveryRate /= SpreadSettings.CrouchMultiplier;
	}
	if (OwnerASC->HasMatchingGameplayTag(FBCTags::AimingDownSights))
	{
		TargetSpread *= SpreadSettings.ADSMultiplier;
		RecoveryRate /= SpreadSettings.ADSMultiplier;
	}
	
	if (TargetSpread > CurrentWeaponSpread)
	{
		CurrentWeaponSpread = TargetSpread;
	}
	else
	{
		CurrentWeaponSpread = FMath::FInterpConstantTo(CurrentWeaponSpread, TargetSpread, DeltaTime, RecoveryRate);
	}
}

void AWeaponBase::ResetFireDelay()
{
	CurrentFireDelay = GetWeaponItemData()->GetFireDelay();
}

void AWeaponBase::OnRep_ItemData()
{
	Super::OnRep_ItemData();
	WeaponItemData = Cast<UWeaponItemData>(ItemData);
}



