// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/FBCGameplayAbility.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Data/StructureInfoDataAsset.h"
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

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// Abilities with this tag are cancelled when this ability is executed and a structure is successfully selected
	UPROPERTY(EditDefaultsOnly, Category = "Tags", meta=(DisplayName="Cancel Abilities with Tag After Selected Structure"))
	FGameplayTagContainer SuccessfulActivationCancelTags{};

	UPROPERTY(EditDefaultsOnly, Category = "Editing")
	TObjectPtr<UStructureInfoDataAsset> StructureInfo{};
	
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TMap<FGameplayTag, TSubclassOf<AEditTargetingActor>> TargetingActors{};

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float Range{};

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	FGameplayTag StartSelectionTag{};

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	FGameplayTag EndSelectionTag{};

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	FGameplayTag ResetEditTag{};

private:
	APlacedStructure* GetSelectedStructure() const;
	TObjectPtr<APlacedStructure> SelectedStructure;

	const FEditMap* CurrentEditMap{};
	
	AEditTargetingActor* SpawnTargetingActor() const;
	TObjectPtr<AEditTargetingActor> TargetingActor;

	UFUNCTION()
	void OnEditDataReceived(const FGameplayAbilityTargetDataHandle& Data);
	
	UFUNCTION()
	void StartSelection(FGameplayEventData Payload);

	UFUNCTION()
	void EndSelection(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnSelectedStructureDestroyed(AActor* DestroyedActor);
	
	UFUNCTION()
	void OnEditReset(FGameplayEventData Payload);
};
