// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/InventoryFastArray.h"
#include "Item/ItemInstance.h"
#include "InventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSlotAddedSignature, int32, SlotIndex, const FItemInstance&, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSlotRemovedSignature, int32, SlotIndex);

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
	};

public:	
	UInventoryComponent();

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnSlotAddedSignature OnSlotAdded;

	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnSlotRemovedSignature OnSlotRemoved;
	
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

private:
	void OnItemRemoved(const FItemInstance& Item, int32 InventoryIndex, int32 SlotIndex);
	void OnItemAdded(const FItemInstance& Item, int32 InventoryIndex, int32 SlotIndex);

	uint8 CurrentInventorySize{};

	UPROPERTY(Replicated)
	FInventoryFastArray Inventory{};
	
	TArray<FInventorySlot> ItemSlots{};
};

