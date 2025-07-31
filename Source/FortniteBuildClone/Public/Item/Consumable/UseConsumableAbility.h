// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "UseConsumableAbility.generated.h"

class UAbilityTask_PlayMontageAndWait;
/**
 * 
 */
UCLASS()
class FORTNITEBUILDCLONE_API UUseConsumableAbility : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	UUseConsumableAbility();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> GameplayEffectClass{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> UsingAnimMontage{};

	UPROPERTY(EditDefaultsOnly)
	FGameplayAttribute MaxRecoverableAttribute{};

	UPROPERTY(EditDefaultsOnly)
	float MaxRecoverableMagnitude{};

private:
	UFUNCTION()
	void OnMontageEnded();
	void OnResourceChanged(const FOnAttributeChangeData& OnAttributeChangeData);
	void OnEffectEnded(const FGameplayEffectRemovalInfo& GameplayEffectRemovalInfo);

	void SetupPeriodicPrediction(FGameplayEffectSpecHandle Spec);
	void ClientOnEffectEnded();
	void ClientOnEffectTick(float Magnitude);

	TObjectPtr<UAbilityTask_PlayMontageAndWait> PlayMontageTask;
	FActiveGameplayEffectHandle ActiveEffectHandle{};


	FTimerHandle ClientEffectDurationTimer{};
	FTimerHandle ClientEffectTickTimer{};
};
