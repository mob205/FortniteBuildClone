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

enum class ESlotAvailability
{
	ESA_None,
	ESA_Empty,
	ESA_Stackable
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FORTNITEBUILDCLONE_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

	struct FInventorySlot
	{
		FItemInstance* Item;
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

	// Gets the next available inventory slot for the specified item type, if any
	// Also checks for item stacking
	ESlotAvailability GetAvailableSlotIndex(const UItemData* ItemData, int32& OutIndex) const;
	
	// Attempts to add an item. Returns true if the item was successfully added
	void ServerTryAddItem(FInstancedStruct ItemInstanceInfo, const UItemData* ItemData);
	
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
	AFBCCharacter* GetAvatarActor();
	
	void OnItemRemoved(FItemInstance& Item, int32 InventoryIndex, int32 SlotIndex);
	void OnItemAdded(FItemInstance& Item, int32 InventoryIndex, int32 SlotIndex);

	void UnequipItem(int32 SlotIndex);
	void EquipItem(int32 SlotIndex);

	void MarkItemDirty(int32 InventoryIndex);
	
	UFUNCTION()
	void OnSelectedItemChanged(uint8 LastSelection);

	uint8 GetSlotCount(const FInstancedStruct& ItemInstanceInfo) const;
	void IncrementSlotCount(FInstancedStruct& ItemInstanceInfo) const;

	UPROPERTY()
	TObjectPtr<AFBCCharacter> AvatarActor{};
	
	uint8 CurrentInventorySize{};

	UPROPERTY(Replicated)
	FInventoryFastArray Inventory{};
	
	TArray<FInventorySlot> ItemSlots{};

	UPROPERTY(ReplicatedUsing = OnSelectedItemChanged)
	uint8 SelectedSlot{};
};