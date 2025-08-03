// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/General/EquippedItemActor.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "Item/Weapon/WeaponItemData.h"
#include "WeaponBase.generated.h"

class UAbilitySystemComponent;
class UWeaponItemData;

UCLASS()
class FORTNITEBUILDCLONE_API AWeaponBase : public AEquippedItemActor
{
	GENERATED_BODY()

public:
	AWeaponBase();

	const UWeaponItemData* GetWeaponItemData() const { return WeaponItemData; }
	
	UFUNCTION(BlueprintCallable)
	void SetAmmoCount(int32 NewAmmoCount) { CurrentAmmo = NewAmmoCount; }

	UFUNCTION(BlueprintCallable)
	void MarkAmmoDirty() const;
	
	virtual void OnItemEquipped(AFBCCharacterBase* AvatarActor) override;
	virtual void OnItemUnequipped(AFBCCharacterBase* AvatarActor) override;
	
	void SetWantsToShoot(bool bNewWantsToShoot);

	UFUNCTION(BlueprintCallable)
	void ResetFireDelay();

	UFUNCTION(BlueprintCallable)
	float GetCurrentFireDelay() const { return CurrentFireDelay; }
	
	UFUNCTION(BlueprintCallable)
	float GetCurrentWeaponSpread() const { return CurrentWeaponSpread; }

	UFUNCTION(BlueprintCallable)
	void HandleFireSpreadIncrease();

	FRandomStream& GetSpreadStream() { return SpreadStream; }
protected:
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetItemData(const UItemData* NewItemData) override;
	virtual void OnRep_ItemData() override;
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentAmmo)
	uint16 CurrentAmmo{};

	UFUNCTION()
	void OnRep_CurrentAmmo();

	UFUNCTION()
	void OnRep_SpreadSeed();
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void OnFireDown(const FGameplayEventData* GameplayEventData);
	void OnFireReleased(const FGameplayEventData* GameplayEventData);
	
	void TryWeaponFire();
	void UpdateSpread(float DeltaTime);

	TObjectPtr<AFBCCharacterBase> Owner;
	TObjectPtr<UAbilitySystemComponent> OwnerASC;
	TObjectPtr<const UWeaponItemData> WeaponItemData;

	UPROPERTY(ReplicatedUsing=OnRep_SpreadSeed)
	int32 SpreadSeed{};

	FRandomStream SpreadStream{};
	
	bool bWantsToShoot{};
	float CurrentFireDelay{};
	float CurrentWeaponSpread{};
};
