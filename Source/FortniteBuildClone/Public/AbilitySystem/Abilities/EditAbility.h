// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FBCGameplayAbility.h"
#include "Abilities/GameplayAbility.h"
#include "EditAbility.generated.h"

class APlacedStructure;

/**
 * 
 */
UCLASS()
class FORTNITEBUILDCLONE_API UEditAbility : public UFBCGameplayAbility
{
	GENERATED_BODY()

protected:
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	// Abilities with this tag are cancelled when this ability is executed and a structure is successfully selected
	UPROPERTY(EditDefaultsOnly, Category = "Tags", meta=(DisplayName="Cancel Abilities with Tag After Selected Structure"))
	FGameplayTagContainer SuccessfulActivationCancelTags{};

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float Range{};

private:
	APlacedStructure* GetSelectedStructure() const;

	TObjectPtr<APlacedStructure> SelectedStructure;

	UFUNCTION()
	void OnConfirm();
};
