// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FireWeaponHitscanAbility.generated.h"

struct FWeaponTargetData;
class AFBCCharacterBase;
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
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UPROPERTY(EditDefaultsOnly)
	float Range{999999};

	UPROPERTY(EditDefaultsOnly)
	float ServerEarlyFireThreshold{.1};

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> OnHitEffectClass;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag GameplayCueTag;

	UPROPERTY(EditDefaultsOnly)
	bool bDebugUseLagCompensation{true};

private:
	TObjectPtr<AWeaponBase> Weapon;
	TObjectPtr<AFBCCharacterBase> FBCOwner;
	TObjectPtr<AGameStateBase> GameState;
	
	void EndAbilityLocally();

	FGameplayAbilityTargetDataHandle GetAimingTargetData() const;
	void TryStoredData();

	void OnValidData(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag GameplayTag);

	void ServerFire(const FWeaponTargetData& TargetData) const;

private:
	FGameplayAbilityTargetDataHandle StoredDataHandle{};
};
