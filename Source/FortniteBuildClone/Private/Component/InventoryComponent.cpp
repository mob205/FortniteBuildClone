// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/InventoryComponent.h"

#include "FortniteBuildClone/FortniteBuildClone.h"
#include "GameFramework/PlayerState.h"
#include "Inventory/ItemData.h"
#include "Inventory/WorldDropActor.h"
#include "Inventory/Items/CountableItem.h"
#include "Inventory/Items/EquippedItemActor.h"
#include "Net/UnrealNetwork.h"
#include "Player/FBCCharacter.h"


UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UInventoryComponent::ServerTryAddItem(AEquippedItemActor* ItemActor)
{
	if (!IsValid(ItemActor)) { return false; }

	// TODO: Need to handle the case when the incoming item has a count higher than 1 - may affect availability
	// Idea: split the item into separate sub-items, each with 1 count, and add the subobjects separately
	
	int32 SlotIndex;
	ACountableItem* AsCountable = Cast<ACountableItem>(ItemActor);

	int32 InItemCount = 1;
	if (AsCountable)
	{
		InItemCount = AsCountable->GetCount();
	}
	int32 StartItemCount = InItemCount;

	while (InItemCount > 0)
	{
		// Attempt to add item to existing stack
		if (AsCountable && GetAvailableSlotIndex(AsCountable, SlotIndex))
		{
			if (ACountableItem* CountableItemSlot = Cast<ACountableItem>(ItemSlots[SlotIndex]))
			{
				CountableItemSlot->SetCount(CountableItemSlot->GetCount() + 1);
			}
		}
		// Attempt to add item to empty slot
		else if (GetAvailableSlotIndex(SlotIndex))
		{
			AddItemToSlot(ItemActor, SlotIndex, InItemCount);
			return true;
		}
		// No space found
		else
		{
			break;
		}
		--InItemCount;
	}

	// We could not fully add the item
	if (InItemCount > 0)
	{
		// We took at least one stack of the item - keep the world drop, but reduce its count
		if (AsCountable && StartItemCount < InItemCount)
		{
			AsCountable->SetCount(InItemCount);
			return false;
		}
		
		// Try to replace the incoming item with our currently equipped item
		// No point swapping if it's the same item type
		if (CanDropItem(SelectedSlot)
			&& ItemActor->GetItemData() != ItemSlots[SelectedSlot]->GetItemData())
		{
			ServerDropFromInventory(SelectedSlot);
			AddItemToSlot(ItemActor, SelectedSlot, InItemCount);
			return true;
		}

		// No way to add it to the inventory
		return false;
	}

	// We successfully added the item, but it combined with existing items
	// We only need to keep the item actor if it gets its own slot
	ItemActor->Destroy();
	
	return true;
}

void UInventoryComponent::AddItemToSlot(AEquippedItemActor* ItemActor, int32 SlotIndex, int32 InItemCount)
{
	ItemActor->SetOwner(GetOwner());
	Inventory.AddItem(ItemActor, SlotIndex);
	OnItemAdded(ItemActor, SlotIndex);

	// InItemCount is how many items we have left to divvy up, so put it all in the new stack
	if (ACountableItem* AsCountable = Cast<ACountableItem>(ItemActor))
	{
		AsCountable->SetCount(InItemCount);
	}
}

AEquippedItemActor* UInventoryComponent::GetItem(int32 SlotIndex) const
{
	if (SlotIndex < 0 || SlotIndex >= MaxInventorySize) { return nullptr; }

	AEquippedItemActor* Item = ItemSlots[SlotIndex];
	return IsValid(Item) ? Item : nullptr;
}

bool UInventoryComponent::IsInventoryFull() const
{
	return CurrentInventorySize < MaxInventorySize;
}

bool UInventoryComponent::CanDropItem(int32 SlotIndex) const
{
	AEquippedItemActor* Item = GetItem(SlotIndex);
	// In future, check some bool for if the slot can't be dropped (like pickaxe slot)
	return IsValid(Item);
}

bool UInventoryComponent::GetAvailableSlotIndex(const ACountableItem* ItemToCheck, int32& OutIndex) const
{
	for (int i = 0; i < MaxInventorySize; ++i)
	{
		ACountableItem* Item = Cast<ACountableItem>(GetItem(i));
		if (IsValid(Item))
		{
			const UItemData* Data = Item->GetItemData();
			if (Data == ItemToCheck->GetItemData() && Item->GetCount() < Data->GetMaxStackSize())
			{
				OutIndex = i;
				return true;
			}
		}
	}
	return false;
}
bool UInventoryComponent::GetAvailableSlotIndex(int32& OutIndex) const
{
	for (int i = 0; i < MaxInventorySize; i++)
	{
		AEquippedItemActor* Item = GetItem(i);

		// Slot is available if it's empty
		if (!IsValid(Item))
		{
			OutIndex = i;
			return true;
		}
	}
	return false;
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
	if (SelectedSlot != NewSelection && NewSelection < MaxInventorySize && IsValid(ItemSlots[NewSelection]))
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

void UInventoryComponent::OnItemAdded(AEquippedItemActor* Item, int32 SlotIndex)
{
	if (!IsValid(Item))
	{
		UE_LOG(LogFBC, Error, TEXT("InventoryComponent: Received invalid item!"));
		return;
	}
	ItemSlots[SlotIndex] = Item;
	
	Item->OnRequestRemoveFromInventory.BindLambda(
		[this, SlotIndex]
		{
			DestroyFromInventory(SlotIndex);
		});
	
	if (SelectedSlot == SlotIndex)
	{
		EquipItem(SlotIndex);
	}
	else
	{
		UnequipItem(SlotIndex);
	}
	OnSlotUpdated.Broadcast(SlotIndex, Item);
}

void UInventoryComponent::OnItemRemoved(AEquippedItemActor* Item, int32 SlotIndex)
{
	if (IsValid(Item))
	{
		if (SelectedSlot == SlotIndex)
		{
			UnequipItem(SlotIndex);
		}
		Item->OnRequestRemoveFromInventory.Unbind();
	}
	ItemSlots[SlotIndex] = {};
	OnSlotUpdated.Broadcast(SlotIndex, nullptr);
}

void UInventoryComponent::UnequipItem(int32 SlotIndex)
{
	if (AEquippedItemActor* Item = GetItem(SlotIndex))
	{
		Item->SetActorHiddenInGame(true);
		Item->OnItemUnequipped(GetAvatarActor());
	}
}

void UInventoryComponent::EquipItem(int32 SlotIndex)
{
	if (AEquippedItemActor* Item = GetItem(SlotIndex))
	{
		Item->SetActorHiddenInGame(false);
		Item->OnItemEquipped(GetAvatarActor());
	}
}

void UInventoryComponent::OnSelectedItemChanged(uint8 LastSelection)
{
	UnequipItem(LastSelection);
	EquipItem(SelectedSlot);
	
	OnSelectedSlotChanged.Broadcast(LastSelection, SelectedSlot);
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

void UInventoryComponent::ClientRequestDropFromInventory(int32 SlotIndex)
{
	if (IsValid(GetItem(SlotIndex)))
	{
		ServerDropFromInventory(SlotIndex);
	}
}

void UInventoryComponent::ServerDropFromInventory_Implementation(uint8 SlotIndex)
{
	if (AEquippedItemActor* Item = GetItem(SlotIndex))
	{
		OnItemRemoved(Item, SlotIndex);

		// OnItemRemoved may delete the item by ending abilities that remove the item from the inventory
		if (!IsValid(Item)) { return; }
		
		Inventory.RemoveItem(SlotIndex);

		TransferToWorldDrop(Item);
	}
}

void UInventoryComponent::TransferToWorldDrop(AEquippedItemActor* Item)
{
	// TODO: Currently just tries to spawn the item at the player's feet. Need to make this better in the future

	FVector AvatarPos = AvatarActor->GetActorLocation();
	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, AvatarPos, AvatarPos+ 200 * FVector::DownVector, ECC_Visibility, AvatarActor->GetIgnoreCharacterParams());
	FVector ItemLocation = Hit.bBlockingHit ? Hit.Location : AvatarPos;
		
	AWorldDropActor* DropActor = GetWorld()->SpawnActor<AWorldDropActor>(
		WorldDropActorClass,
		ItemLocation, 
		{});
	DropActor->InitializeFromItemActor(Item);
	Item->SetOwner(DropActor);
}

void UInventoryComponent::DestroyFromInventory(int32 SlotIndex)
{
	if (AEquippedItemActor* Item = GetItem(SlotIndex))
	{
		OnItemRemoved(Item, SlotIndex);
		
		if (GetOwner()->HasAuthority())
		{
			Inventory.RemoveItem(Item);
			Item->Destroy();
		}
	}
}
