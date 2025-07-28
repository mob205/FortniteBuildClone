// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FireWeaponHitscanAbility.generated.h"

struct FWeaponTargetData;
class AFBCCharacter;
class AWeaponBase;

UCLASS()
class FORTNITEBUILDCLONE_API UFireWeaponHitscanAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UFireWeaponHitscanAbility();
	
protected:
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly)
	float Range{999999};

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> OnHitEffectClass;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag GameplayCueTag;

private:
	TObjectPtr<AWeaponBase> Weapon;
	TObjectPtr<AFBCCharacter> FBCOwner;
	TObjectPtr<AGameStateBase> GameState;
	
	void EndAbilityLocally();

	FGameplayAbilityTargetDataHandle GetAimingTargetData() const;

	void OnValidData(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag GameplayTag);

	void ServerFire(const FWeaponTargetData& TargetData) const;
};
