// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "SprintAbility.generated.h"

class UFBCCharacterMovementComponent;

UCLASS()
class FORTNITEBUILDCLONE_API USprintAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> StartEffectClass{};

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> EndEffectClass{};

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
private:
	void CheckStamina(const FOnAttributeChangeData& Data);

	TObjectPtr<UFBCCharacterMovementComponent> CMC;

	void SetSprintStatus(bool bSprinting) const;
};
