// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/InventoryFastArray.h"
#include "Item/ItemInstance.h"
#include "InventoryComponent.generated.h"

class AFBCCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlotAddedSignature, int32, SlotIndex, const FItemInstance&, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotRemovedSignature, int32, SlotIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSelectedSlotChangedSignature, int32, OldSlotIndex, int32, NewSlotIndex);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FORTNITEBUILDCLONE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

	struct FInventorySlot
	{
		const FItemInstance* Item;
		int32 InventoryIndex;

		bool operator==(const FInventorySlot& Other) const
		{
			return Item == Other.Item && InventoryIndex == Other.InventoryIndex;
		}
		bool IsEmpty() const
		{
			return Item == nullptr;
		}
	};

public:	
	UInventoryComponent();

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnSlotAddedSignature OnSlotAdded;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnSlotRemovedSignature OnSlotRemoved;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnSelectedSlotChangedSignature OnSelectedSlotChanged;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	uint8 GetCurrentSelectedSlot() const { return SelectedSlot; }
	
	// Returns true if any item can be added
	bool CanAddItem() const;

	int32 GetAvailableSlotIndex() const;
	
	// Attempts to add an item. Returns true if the item was successfully added
	UFUNCTION(Server, Reliable)
	void TryAddItem(FInstancedStruct ItemInstanceInfo, const UItemData* ItemData);
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	uint8 MaxInventorySize{};

	UFUNCTION(Server, Reliable)
	void ServerRequestSwitchItem(uint8 NewSelection);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void ClientTryEquipItem(int32 NewSelection);
private:
	void OnItemRemoved(const FItemInstance& Item, int32 InventoryIndex, int32 SlotIndex);
	void OnItemAdded(const FItemInstance& Item, int32 InventoryIndex, int32 SlotIndex);

	void UnequipItem(int32 SlotIndex);
	void EquipItem(int32 SlotIndex);

	void FinalizeItemAdd(int32 SlotIndex);
	
	UFUNCTION()
	void OnSelectedItemChanged(uint8 LastSelection);

	UPROPERTY()
	TObjectPtr<AFBCCharacter> AvatarActor{};
	
	uint8 CurrentInventorySize{};

	UPROPERTY(Replicated)
	FInventoryFastArray Inventory{};
	
	TArray<FInventorySlot> ItemSlots{};

	UPROPERTY(ReplicatedUsing = OnSelectedItemChanged)
	uint8 SelectedSlot{};
};

