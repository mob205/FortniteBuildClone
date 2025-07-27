// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FireWeaponHitscan.generated.h"

class AWeaponBase;

UCLASS()
class FORTNITEBUILDCLONE_API UFireWeaponHitscan : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UFireWeaponHitscan();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	TObjectPtr<AWeaponBase> Weapon;

	UPROPERTY(EditDefaultsOnly)
	float Range{999999};

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> OnHitEffectClass;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag GameplayCueTag;

private:
	UFUNCTION()
	void OnValidData(const FGameplayAbilityTargetDataHandle& Data);
};
