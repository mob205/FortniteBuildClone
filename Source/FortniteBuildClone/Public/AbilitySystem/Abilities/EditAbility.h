// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FBCGameplayAbility.h"
#include "Abilities/GameplayAbility.h"
#include "EditAbility.generated.h"

class APlacedStructure;
class AEditTargetingActor;

/**
 * 
 */
UCLASS()
class FORTNITEBUILDCLONE_API UEditAbility : public UFBCGameplayAbility
{
	GENERATED_BODY()

public:
	UEditAbility();
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	// Abilities with this tag are cancelled when this ability is executed and a structure is successfully selected
	UPROPERTY(EditDefaultsOnly, Category = "Tags", meta=(DisplayName="Cancel Abilities with Tag After Selected Structure"))
	FGameplayTagContainer SuccessfulActivationCancelTags{};

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TMap<FGameplayTag, TSubclassOf<AEditTargetingActor>> TargetingActors{};

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float Range{};

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	FGameplayTag StartSelectionTag{};

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	FGameplayTag EndSelectionTag{};
	
private:
	APlacedStructure* GetSelectedStructure() const;
	TObjectPtr<APlacedStructure> SelectedStructure;

	// Tracks whether the caster has the selection input held down
	// Selecting is used to select/deselect edit actor tiles
	bool bIsSelecting{};

	AEditTargetingActor* SpawnTargetingActor() const;
	TObjectPtr<AEditTargetingActor> TargetingActor;
	
	UFUNCTION()
	void OnConfirm();

	UFUNCTION()
	void StartSelection(FGameplayEventData Payload);

	UFUNCTION()
	void EndSelection(FGameplayEventData Payload);
};
