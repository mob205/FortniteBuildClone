// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FBCGameplayAbility.generated.h"

class UInputMappingContext;
class UEnhancedInputLocalPlayerSubsystem;

/**
 * 
 */
UCLASS()
class FORTNITEBUILDCLONE_API UFBCGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> AbilityMappingContext{};
	
	void AddAbilityInputMappingContext();
	void RemoveAbilityInputMappingContext();

private:
	TObjectPtr<UEnhancedInputLocalPlayerSubsystem> AvatarInputSubsystem{};
};
