// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/FireWeaponHitscan.h"

#include "AbilitySystemComponent.h"
#include "FBCBlueprintLibrary.h"
#include "AbilitySystem/GameplayTags/FBCTags.h"
#include "FortniteBuildClone/FortniteBuildClone.h"
#include "Item/Weapon/WeaponBase.h"
#include "Player/FBCCharacter.h"
#include "Player/FBCPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/GameStateBase.h"
#include "Item/Weapon/WeaponTargetData.h"

UFireWeaponHitscan::UFireWeaponHitscan()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	SetAssetTags(FBCTags::FireWeapon.GetTag().GetSingleTagContainer());
}

void UFireWeaponHitscan::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	FBCOwner = Cast<AFBCCharacter>(ActorInfo->AvatarActor);
	GameState = GetWorld()->GetGameState();
}

void UFireWeaponHitscan::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	Weapon = Cast<AWeaponBase>(GetCurrentSourceObject());
	
	if (!Weapon)
	{
		UE_LOG(LogFBC, Error, TEXT("FireWeaponHitscan: No weapon source object"));
		CancelAbility(Handle, ActorInfo, ActivationInfo, false);
		return;
	}

	if (Weapon->GetCurrentFireDelay() > 0)
	{
		UE_LOG(LogFBC, Error, TEXT("FireWeaponHitscan: Weapon has active fire delay"));
		CancelAbility(Handle, ActorInfo, ActivationInfo, false);
		return;
	}

	Weapon->ResetFireDelay();
	
	if (HasAuthority(&ActivationInfo))
	{
		ASC->AbilityTargetDataSetDelegate(CurrentSpecHandle, ActivationInfo.GetActivationPredictionKey()).AddUObject(this, &UFireWeaponHitscan::OnValidData);
	}
	else
	{
		ASC->ServerSetReplicatedTargetData(CurrentSpecHandle, ActivationInfo.GetActivationPredictionKey(), GetAimingTargetData(), {}, ActivationInfo.GetActivationPredictionKey());
	}

	// Wait until next tick to give GAS a chance to send the target data RPC
	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ThisClass::EndAbilityLocally));
}

void UFireWeaponHitscan::EndAbilityLocally()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

FGameplayAbilityTargetDataHandle UFireWeaponHitscan::GetAimingTargetData() const
{
	double Timestamp = GameState->GetServerWorldTimeSeconds();
	FVector ViewLocation{};
	FRotator ViewRotation{};
	FBCOwner->GetPlayerController()->GetPlayerViewPoint(ViewLocation, ViewRotation);
	
	return { new FWeaponTargetData{Timestamp, ViewLocation, ViewRotation} };
}


// Reconstruct the client shot on the server
void UFireWeaponHitscan::OnValidData(const FGameplayAbilityTargetDataHandle& Data,
                                     FGameplayTag GameplayTag)
{
	const FWeaponTargetData* ShotData = static_cast<const FWeaponTargetData*>(Data.Get(0));

	FVector Start = ShotData->ViewLocation;
	FVector End = ShotData->ViewLocation + Range * ShotData->ViewRotation.Vector();
	FHitResult Hit;
	
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, FBCOwner->GetIgnoreCharacterParams()))
	{
		if (UAbilitySystemComponent* HitASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Hit.GetActor()))
		{
			// FGameplayEffectContextHandle ContextHandle = HitASC->MakeEffectContext();
			// FGameplayEffectSpecHandle SpecHandle = HitASC->MakeOutgoingSpec(OnHitEffectClass, 1, ContextHandle);
			// HitASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
		}
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}
