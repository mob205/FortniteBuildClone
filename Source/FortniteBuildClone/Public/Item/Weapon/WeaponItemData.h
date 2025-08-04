// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/General/ItemData.h"
#include "WeaponItemData.generated.h"

USTRUCT(BlueprintType)
struct FSpreadSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Spread")
	float BaseAngle{};

	UPROPERTY(EditDefaultsOnly, Category = "Spread")
	float MaxAngle{};

	UPROPERTY(EditDefaultsOnly, Category = "Spread")
	float IncreasePerShot{};

	UPROPERTY(EditDefaultsOnly, Category = "Spread")
	float RecoveryRate{};

	UPROPERTY(EditDefaultsOnly, Category = "Spread")
	float ADSMultiplier{};

	UPROPERTY(EditDefaultsOnly, Category = "Spread")
	float CrouchMultiplier{};

	UPROPERTY(EditDefaultsOnly, Category = "Spread")
	float AirborneMultiplier{};
	
	UPROPERTY(EditDefaultsOnly, Category = "Spread")
	float MovementPenalty{};
};

UCLASS()
class FORTNITEBUILDCLONE_API UWeaponItemData : public UItemData
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	int32 GetMaxAmmoCount() const { return MaxAmmoCount; }

	UFUNCTION(BlueprintCallable)
	float GetFireDelay() const { return FireDelay; }

	UFUNCTION(BlueprintCallable)
	float GetDamage() const { return Damage; }

	UFUNCTION(BlueprintCallable)
	float GetReloadDelay() const { return ReloadDelay; }
	
	UFUNCTION(BlueprintCallable)
	const FSpreadSettings& GetSpreadSettings() const { return SpreadSettings; }
protected:
	// Weapon
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int32 MaxAmmoCount{};

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float FireDelay{};

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float Damage{};

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float ReloadDelay{};
	
	// Spread
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FSpreadSettings SpreadSettings{};
};
