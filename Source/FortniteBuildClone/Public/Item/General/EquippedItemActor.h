// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameFramework/Actor.h"
#include "EquippedItemActor.generated.h"

class UItemData;
class UGameplayAbility;
class AFBCCharacterBase;

DECLARE_DELEGATE(FOnRequestRemoveFromInventorySignature);

UCLASS()
class FORTNITEBUILDCLONE_API AEquippedItemActor : public AActor
{
	GENERATED_BODY()

public:
	AEquippedItemActor();

	FOnRequestRemoveFromInventorySignature OnRequestRemoveFromInventory;
	
	virtual void OnItemEquipped(AFBCCharacterBase* Owner);
	virtual void OnItemUnequipped(AFBCCharacterBase* Owner);

	UFUNCTION(BlueprintCallable)
	void RemoveFromOwningInventory() const { OnRequestRemoveFromInventory.ExecuteIfBound(); }

	UFUNCTION(BlueprintCallable)
	const UItemData* GetItemData() const { return ItemData; }
	virtual void SetItemData(const UItemData* NewItemData) { ItemData = NewItemData; }

	UFUNCTION(BlueprintCallable)
	UStaticMeshComponent* GetStaticMesh() const { return StaticMeshComp; }
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	void PlayMontageSection(FName SectionName);
	
	UFUNCTION()
	virtual void OnRep_ItemData() {};
	
	UPROPERTY(Transient)
	TSet<FGameplayAbilitySpecHandle> GrantedAbilityHandles{};

	UPROPERTY(ReplicatedUsing = OnRep_ItemData)
	TObjectPtr<const UItemData> ItemData{};

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> RootComp{};

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMeshComp{};

private:
	TWeakObjectPtr<UAnimInstance> AnimInstance;
};

