// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FBCGameplayAbility.h"
#include "InteractAbility.generated.h"

/**
 * 
 */
UCLASS()
class FORTNITEBUILDCLONE_API UInteractAbility : public UFBCGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UPROPERTY(EditDefaultsOnly)
	float Range{800.f};

	UPROPERTY(EditDefaultsOnly)
	float InteractPollDelay{0.1f};
private:
	TObjectPtr<AActor> InteractActor{};
	FTimerHandle TickHandle{};

	AController* AvatarController{};
	
	void PollInteractable();
};
