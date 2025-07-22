// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameFramework/Actor.h"
#include "EquippedItemActor.generated.h"

class UItemData;
class UGameplayAbility;
class AFBCCharacter;

DECLARE_DELEGATE(FOnRequestRemoveFromInventorySignature);

UCLASS()
class FORTNITEBUILDCLONE_API AEquippedItemActor : public AActor
{
	GENERATED_BODY()

public:
	AEquippedItemActor();

	FOnRequestRemoveFromInventorySignature OnRequestRemoveFromInventory;
	
	virtual void OnItemEquipped(AFBCCharacter* AvatarActor);
	virtual void OnItemUnequipped(AFBCCharacter* AvatarActor);

	UFUNCTION(BlueprintCallable)
	void RemoveFromOwningInventory() const { OnRequestRemoveFromInventory.ExecuteIfBound(); }

	UFUNCTION(BlueprintCallable)
	const UItemData* GetItemData() const { return ItemData; }
	void SetItemData(const UItemData* NewItemData) { ItemData = NewItemData;}
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(EditDefaultsOnly)
	FName SocketName{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> EquipMontage{};

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities{};

	UPROPERTY(Transient)
	TSet<FGameplayAbilitySpecHandle> GrantedAbilityHandles{};

	UPROPERTY(Replicated)
	TObjectPtr<const UItemData> ItemData{};
};

