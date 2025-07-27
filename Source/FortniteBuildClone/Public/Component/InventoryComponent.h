// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/General/InventoryFastArray.h"
#include "InventoryComponent.generated.h"

class AWorldDropActor;
class ACountableItem;
class UItemData;
class AFBCCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlotUpdatedSignature, int32, SlotIndex, const AEquippedItemActor*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSelectedSlotChangedSignature, int32, OldSlotIndex, int32, NewSlotIndex);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FORTNITEBUILDCLONE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();
	void AddItemToSlot(AEquippedItemActor* ItemActor, int32 SlotIndex, int32 InItemCount);

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnSlotUpdatedSignature OnSlotUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnSelectedSlotChangedSignature OnSelectedSlotChanged;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	uint8 GetCurrentSelectedSlot() const { return SelectedSlot; }

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	AEquippedItemActor* GetItem(int32 SlotIndex) const;
	
	// Returns true if inventory is full. Does not consider item stacking
	bool IsInventoryFull() const;

	bool CanDropItem(int32 SlotIndex) const;

	// Gets the next available inventory slot for the specified item type, if any
	// Also checks for item stacking
	bool GetAvailableSlotIndex(int32& OutIndex) const;

	bool GetAvailableSlotIndex(const ACountableItem* ItemToCheck, int32& OutIndex) const;
	
	// Attempts to add an item. Returns true if the item was successfully added
	bool ServerTryAddItem(AEquippedItemActor* ItemActor);
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 MaxInventorySize{};

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AWorldDropActor> WorldDropActorClass;

	UFUNCTION(Server, Reliable)
	void ServerRequestSwitchItem(uint8 NewSelection);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ClientRequestDropFromInventory(int32 SlotIndex);

	UFUNCTION(Server, Reliable)
	void ServerDropFromInventory(uint8 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ClientTryEquipItem(int32 NewSelection);
private:
	AFBCCharacter* GetAvatarActor();

	void DestroyFromInventory(int32 SlotIndex);
	
	void OnItemRemoved(AEquippedItemActor* Item, int32 SlotIndex);
	void OnItemAdded(AEquippedItemActor* Item, int32 SlotIndex);
	
	void UnequipItem(int32 SlotIndex);
	void EquipItem(int32 SlotIndex);

	// Creates a world drop actor and transfers ownership of the item to the world drop actor
	void TransferToWorldDrop(AEquippedItemActor* Item);
	
	UFUNCTION()
	void OnSelectedItemChanged(uint8 LastSelection);

	UPROPERTY()
	TObjectPtr<AFBCCharacter> AvatarActor{};
	
	uint8 CurrentInventorySize{};

	UPROPERTY(Replicated)
	FInventoryFastArray Inventory{};
	
	TArray<AEquippedItemActor*> ItemSlots{};

	UPROPERTY(ReplicatedUsing = OnSelectedItemChanged)
	uint8 SelectedSlot{};
};
