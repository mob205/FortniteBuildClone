// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquippedItemActor.h"
#include "GameplayTagContainer.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Inventory/Data/WeaponItemData.h"
#include "WeaponBase.generated.h"

class UAbilitySystemComponent;
class UWeaponItemData;

UCLASS()
class FORTNITEBUILDCLONE_API AWeaponBase : public AEquippedItemActor
{
	GENERATED_BODY()

public:
	AWeaponBase();

	const UWeaponItemData* GetWeaponItemData() const { return Cast<UWeaponItemData>(ItemData); } 
	UFUNCTION(BlueprintCallable)
	void SetAmmoCount(int32 NewAmmoCount) { CurrentAmmo = NewAmmoCount; }

	UFUNCTION(BlueprintCallable)
	void MarkAmmoDirty() const;
	
	virtual void OnItemEquipped(AFBCCharacter* AvatarActor) override;
	virtual void OnItemUnequipped(AFBCCharacter* AvatarActor) override;
	
	void SetWantsToShoot(bool bNewWantsToShoot);

	UFUNCTION(BlueprintCallable)
	void ResetFireDelay();

	UFUNCTION(BlueprintCallable)
	float GetCurrentFireDelay() const { return CurrentFireDelay; }
protected:
	virtual void Tick(float DeltaSeconds) override;
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentAmmo)
	uint16 CurrentAmmo{};

	UFUNCTION()
	void OnRep_CurrentAmmo();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void OnFireDown(const FGameplayEventData* GameplayEventData);
	void OnFireReleased(const FGameplayEventData* GameplayEventData);
	
	bool bWantsToShoot{};
	TObjectPtr<UAbilitySystemComponent> OwnerASC;
	
	float CurrentFireDelay{};
	void TryWeaponFire();
};
