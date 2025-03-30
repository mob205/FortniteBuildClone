// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "BuildAbility.generated.h"

class AStructureTargetingActor;
class UStructureInfoDataAsset;
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Targeting")
	float TargetingRange{1000};

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
private:
	TObjectPtr<AStructureTargetingActor> TargetingActor{};

	FGameplayTag SelectedStructureTag{};

	UFUNCTION()
	void PlaceStructure(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void CallEndAbility(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void RotateTargetingActor(FGameplayEventData Payload);

	UFUNCTION()
	void OnSelectStructure(FGameplayEventData Payload);
};

