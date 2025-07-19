// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/InventoryFastArray.h"
#include "Item/ItemSlot.h"
#include "InventoryComponent.generated.h"

class UItemSlot;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotRemovedSignature, int32, SlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlotAddedSignature, int32, SlotIndex, UItemSlot*, ItemSlot);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FORTNITEBUILDCLONE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FOnSlotRemovedSignature OnSlotRemoved;
	
	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FOnSlotAddedSignature OnSlotAdded;
	
	// Returns true if any item can be added
	bool CanAddItem() const;

	int32 GetAvailableSlotIndex() const;
	
	// Attempts to add an item. Returns true if the item was successfully added
	UFUNCTION(Server, Reliable)
	void TryAddItem(FItemInstance Item);
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 MaxInventorySize{};

private:
	void OnItemRemoved(const FItemInstance& Item, AActor* Actor, int32 SlotIndex);
	void OnItemAdded(const FItemInstance& Item, AActor* Actor, int32 SlotIndex);
	void OnItemChanged(const FItemInstance& Item, AActor* Actor, int32 SlotIndex);

	void UpdateSlots();
	void RemoveSlot(int32 SlotIndex);
	void AddSlot(int32 SlotIndex, const UClass* SlotClass, const FItemInstance& Item);
	
	uint8 CurrentInventorySize{};

	UPROPERTY(Replicated)
	FInventoryFastArray Inventory{};
	
	UPROPERTY()
	TArray<TObjectPtr<UItemSlot>> ItemSlots{};
};

