// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemData.h"
#include "WeaponItemData.generated.h"

UCLASS()
class FORTNITEBUILDCLONE_API UWeaponItemData : public UItemData
{
	GENERATED_BODY()

	
public:
	UFUNCTION(BlueprintCallable)
	int32 GetMaxAmmoCount() const { return MaxAmmoCount; }

	UFUNCTION(BlueprintCallable)
	float GetFireDelay() const { return FireDelay;}
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int32 MaxAmmoCount{};

	UPROPERTY(EditDefaultsOnly, Category = "Fire Delay")
	float FireDelay{};
};
