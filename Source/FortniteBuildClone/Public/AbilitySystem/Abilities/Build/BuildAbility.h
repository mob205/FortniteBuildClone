// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/FBCGameplayAbility.h"
#include "Abilities/GameplayAbility.h"
#include "Data/BitGrid.h"
#include "Data/StructureResourceTypes.h"
#include "Subsystem/StructureStrategyWorldSubsystem.h"
#include "BuildAbility.generated.h"

class UEnhancedInputLocalPlayerSubsystem;
class AStructureTargetingActor;
class UStructureInfoDataAsset;
class UBuildResourceComponent;

/**
 * 
 */
UCLASS()
class FORTNITEBUILDCLONE_API UBuildAbility : public UFBCGameplayAbility
{
	GENERATED_BODY()

public:
	UBuildAbility();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
	TObjectPtr<UStructureInfoDataAsset> StructureInfo{};

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	TSubclassOf<AStructureTargetingActor> TargetingActorClass{};

	virtual UGameplayEffect* GetCostGameplayEffect() const override;
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UPROPERTY(EditDefaultsOnly, Category = "Costs")
	TMap<EFBCResourceType, TSubclassOf<UGameplayEffect>> MaterialCostEffects{};

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
private:
	TObjectPtr<AStructureTargetingActor> TargetingActor{};
	TObjectPtr<UStructureStrategyWorldSubsystem> StrategyWorldSubsystem{};

	EFBCResourceType CachedMaterialType{};

	int32 GetCurrentStructureEdit(FGameplayTag StructureTag);
	
	TMap<FGameplayTag, FBitGrid> CurrentStructureEdits{};
	
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

	// Handles when an edit ability is activated while the build ability is ongoing
	void OnStartEdit(FGameplayTag Tag, int32 Count);
	
	UPROPERTY()
	TObjectPtr<AActor> Avatar;

	UPROPERTY()
	TObjectPtr<UBuildResourceComponent> ResourceComponent;
};

