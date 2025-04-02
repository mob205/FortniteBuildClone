// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "BuildAbility.generated.h"

class AStructureTargetingActor;
class UStructureInfoDataAsset;
class UGridWorldSubsystem;
/**
 * 
 */
UCLASS()
class FORTNITEBUILDCLONE_API UBuildAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UBuildAbility();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
	TObjectPtr<UStructureInfoDataAsset> StructureInfo{};

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<AStructureTargetingActor> TargetingActorClass{};

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
private:
	TObjectPtr<AStructureTargetingActor> TargetingActor{};

	TObjectPtr<UGridWorldSubsystem> GridWorldSubsystem{};

	FGameplayTag SelectedStructureTag{};

	// Places a structure after target data is received
	UFUNCTION()
	void PlaceStructure(const FGameplayAbilityTargetDataHandle& Data);

	// Ends the ability, but can bind to FWaitTargetDataDelegate
	UFUNCTION()
	void CallEndAbility(const FGameplayAbilityTargetDataHandle& Data);

	// Alerts the targeting actor of a request to rotate
	UFUNCTION()
	void RotateTargetingActor(FGameplayEventData Payload);

	// Updates targeting actor with the newly selected structure
	UFUNCTION()
	void OnSelectStructure(FGameplayEventData Payload);
};

