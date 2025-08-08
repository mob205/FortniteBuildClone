// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"

#include "Damageable.generated.h"

// This class does not need to be modified.
UINTERFACE(NotBlueprintable, BlueprintType)
class UDamageable : public UInterface
{
	GENERATED_BODY()
};

class FORTNITEBUILDCLONE_API IDamageable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void Damage(FGameplayEffectSpecHandle DamageEffectSpec) {};
};
