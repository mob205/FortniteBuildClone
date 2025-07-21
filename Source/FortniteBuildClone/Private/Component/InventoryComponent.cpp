// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/InventoryComponent.h"

#include "FortniteBuildClone/FortniteBuildClone.h"
#include "GameFramework/PlayerState.h"
#include "Item/EquippedItemActor.h"
#include "Item/ItemData.h"
#include "Net/UnrealNetwork.h"
#include "Player/FBCCharacter.h"


UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryComponent::ServerTryAddItem(FInstancedStruct ItemInstanceInfo, const UItemData* ItemData)
{
	if (!CanAddItem()) { return; }

	// TODO: Need to handle the case when the incoming item has a count higher than 1 - may affect availability
	// Idea: split the item into separate sub-items, each with 1 count, and add the subobjects separately
	
	int32 SlotIndex;
	ESlotAvailability SlotAvailability = GetAvailableSlotIndex(ItemData, SlotIndex);

	if (SlotAvailability == ESlotAvailability::ESA_None) { return; }

	if (SlotAvailability == ESlotAvailability::ESA_Empty)
	{
		// Create new item
		AEquippedItemActor* ItemActor = GetWorld()->SpawnActor<AEquippedItemActor>(ItemData->GetActorClass());
		ItemActor->SetOwner(GetOwner());
	
		FItemInstance ItemInstance = { ItemInstanceInfo, ItemActor, ItemData };
	
		int32 InventoryIndex = Inventory.AddItem(ItemInstance, SlotIndex);
		OnItemAdded(Inventory.GetItem(InventoryIndex).ItemInstance, InventoryIndex, SlotIndex);
	}
	else if (SlotAvailability == ESlotAvailability::ESA_Stackable)
	{
		// Update existing item entry
		IncrementSlotCount(ItemSlots[SlotIndex].Item->InstanceInfo);
		MarkItemDirty(ItemSlots[SlotIndex].InventoryIndex);
	}
}

bool UInventoryComponent::CanAddItem() const
{
	return CurrentInventorySize < MaxInventorySize;
}

ESlotAvailability UInventoryComponent::GetAvailableSlotIndex(const UItemData* ItemData, int32& OutIndex) const
{
	for (int i = 0; i < MaxInventorySize; i++)
	{
		const FInventorySlot& ItemSlot = ItemSlots[i];

		// Slot is available if it's empty OR if the current item can stack with the new one
		if (ItemSlot.IsEmpty())
		{
			OutIndex = i;
			return ESlotAvailability::ESA_Empty;
		}
		if (ItemSlot.Item->ItemData == ItemData && GetSlotCount(ItemSlot.Item->InstanceInfo) < ItemData->GetMaxStackSize())
		{
			OutIndex = i;
			return ESlotAvailability::ESA_Stackable;
		}
	}
	return ESlotAvailability::ESA_None;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, Inventory);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS_FAST(UInventoryComponent, SelectedSlot, Params);
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	Inventory.OnItemRemoved.BindUObject(this, &ThisClass::OnItemRemoved);
	Inventory.OnItemAdded.BindUObject(this, &ThisClass::OnItemAdded);
	
	ItemSlots.Init({}, MaxInventorySize);
}

void UInventoryComponent::ServerRequestSwitchItem_Implementation(uint8 NewSelection)
{
	if (SelectedSlot != NewSelection && NewSelection < MaxInventorySize && !ItemSlots[NewSelection].IsEmpty())
	{
		uint8 LastSelection = SelectedSlot;
		SelectedSlot = NewSelection;
		MARK_PROPERTY_DIRTY_FROM_NAME(UInventoryComponent, SelectedSlot, this);
		OnSelectedItemChanged(LastSelection);
	}
}

void UInventoryComponent::ClientTryEquipItem(int32 NewSelection)
{
	if (0 <= NewSelection && NewSelection < MaxInventorySize)
	{
		ServerRequestSwitchItem(NewSelection);
	}
}

void UInventoryComponent::OnItemRemoved(FItemInstance& Item, int32 InventoryIndex, int32 SlotIndex)
{
	ItemSlots[SlotIndex] = {};
	OnSlotRemoved.Broadcast(SlotIndex);
}

void UInventoryComponent::OnItemAdded(FItemInstance& Item, int32 InventoryIndex, int32 SlotIndex)
{
	ItemSlots[SlotIndex] = { &Item, InventoryIndex };

	// We give the item mutable access to the underlying struct, but it needs to tell us when it wants to replicate
	Item.AssociatedActor->SetItemInfo(&Item.InstanceInfo);
	Item.AssociatedActor->OnItemInformationChanged.BindLambda(
		[this, InventoryIndex]()
		{
			MarkItemDirty(InventoryIndex);
		});
	
	if (SelectedSlot == SlotIndex)
	{
		EquipItem(SlotIndex);
	}
	else
	{
		UnequipItem(SlotIndex);
	}
	
	OnSlotAdded.Broadcast(SlotIndex, *ItemSlots[SlotIndex].Item);
}

void UInventoryComponent::UnequipItem(int32 SlotIndex)
{
	if (SlotIndex >= MaxInventorySize || ItemSlots[SlotIndex].IsEmpty()) { return; }
	
	const FItemInstance& Item = *ItemSlots[SlotIndex].Item;
	Item.AssociatedActor->SetActorHiddenInGame(true);
	Item.AssociatedActor->OnItemUnequipped(GetAvatarActor());
}

void UInventoryComponent::EquipItem(int32 SlotIndex)
{
	if (SlotIndex >= MaxInventorySize || ItemSlots[SlotIndex].IsEmpty()) { return; }
	
	const FItemInstance& Item = *ItemSlots[SlotIndex].Item;
	Item.AssociatedActor->SetActorHiddenInGame(false);
	Item.AssociatedActor->OnItemEquipped(GetAvatarActor());
}


void UInventoryComponent::OnSelectedItemChanged(uint8 LastSelection)
{
	UnequipItem(LastSelection);
	EquipItem(SelectedSlot);
	
	OnSelectedSlotChanged.Broadcast(LastSelection, SelectedSlot);
}

uint8 UInventoryComponent::GetSlotCount(const FInstancedStruct& ItemInstanceInfo) const
{
	// Uses reflection to get slot count from the item instance info
	
	// Assuming that slot counts are always named uint8's named "Count". Can make this more robust if needed
	// 1 is fallback, since just having a valid reference means there is an item
	if (!ItemInstanceInfo.IsValid()) { return 1; }

	FProperty* Prop = ItemInstanceInfo.GetScriptStruct()->FindPropertyByName("Count");
	if (FByteProperty* ByteProp = CastField<FByteProperty>(Prop))
	{
		return ByteProp->GetPropertyValue(ItemInstanceInfo.GetMemory());
	}
	return 1;
}

void UInventoryComponent::IncrementSlotCount(FInstancedStruct& ItemInstanceInfo) const
{
	if (!ItemInstanceInfo.IsValid())
	{
		UE_LOG(LogFBC, Error, TEXT("InventoryComponent: Could not find \"Count\" property on item instance struct."));
		return;
	}

	FProperty* Prop = ItemInstanceInfo.GetScriptStruct()->FindPropertyByName("Count");
	if (FByteProperty* ByteProp = CastField<FByteProperty>(Prop))
	{
		uint8 CurrentValue = ByteProp->GetPropertyValue(ItemInstanceInfo.GetMemory());
		ByteProp->SetPropertyValue(ItemInstanceInfo.GetMutableMemory(), CurrentValue + 1);
	}
	else
	{
		UE_LOG(LogFBC, Error, TEXT("InventoryComponent: Could not find \"Count\" property on item instance struct."));
	}
}

AFBCCharacter* UInventoryComponent::GetAvatarActor()
{
	if (AvatarActor)
	{
		return AvatarActor;
	}
	
	if (APlayerState* OwnerPS = Cast<APlayerState>(GetOwner()))
	{
		AvatarActor = Cast<AFBCCharacter>(OwnerPS->GetPawn());
	}
	return AvatarActor;
}

void UInventoryComponent::MarkItemDirty(int32 InventoryIndex)
{
	Inventory.MarkItemDirty(Inventory.GetItem(InventoryIndex));
}