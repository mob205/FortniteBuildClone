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

bool UInventoryComponent::CanAddItem() const
{
	return CurrentInventorySize < MaxInventorySize;
}

int32 UInventoryComponent::GetAvailableSlotIndex() const
{
	for (int i = 0; i < MaxInventorySize; i++)
	{
		if (ItemSlots[i].IsEmpty())
		{
			return i;
		}
	}
	return -1;
}

void UInventoryComponent::TryAddItem_Implementation(FInstancedStruct ItemInstanceInfo, const UItemData* ItemData)
{
	if (!CanAddItem()) { return; }

	AEquippedItemActor* ItemActor = GetWorld()->SpawnActor<AEquippedItemActor>(ItemData->GetActorClass());
	ItemActor->SetOwner(GetOwner());
	
	int32 SlotIndex = GetAvailableSlotIndex();
	FItemInstance ItemInstance = { ItemInstanceInfo, ItemActor, ItemData };
	
	int32 InventoryIndex = Inventory.AddItem(ItemInstance, SlotIndex);
	OnItemAdded(Inventory.GetItem(InventoryIndex).ItemInstance, InventoryIndex, SlotIndex);
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

	if (APlayerState* OwnerPS = Cast<APlayerState>(GetOwner()))
	{
		AvatarActor = Cast<AFBCCharacter>(OwnerPS->GetPawn());
	}
	else
	{
		UE_LOG(LogFBC, Error, TEXT("Could not find owner actor for inventory component"));
	}
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

void UInventoryComponent::OnItemRemoved(const FItemInstance& Item, int32 InventoryIndex, int32 SlotIndex)
{
	ItemSlots[SlotIndex] = {};
	OnSlotRemoved.Broadcast(SlotIndex);
}

void UInventoryComponent::OnItemAdded(const FItemInstance& Item, int32 InventoryIndex, int32 SlotIndex)
{
	ItemSlots[SlotIndex] = { &Item, InventoryIndex };
	UnequipItem(SlotIndex);
	OnSlotAdded.Broadcast(SlotIndex, *ItemSlots[SlotIndex].Item);
}

void UInventoryComponent::UnequipItem(int32 SlotIndex)
{
	if (SlotIndex >= MaxInventorySize || ItemSlots[SlotIndex].IsEmpty()) { return; }
	
	const FItemInstance& Item = *ItemSlots[SlotIndex].Item;
	Item.AssociatedActor->SetActorHiddenInGame(true);
	Item.AssociatedActor->OnItemUnequipped(AvatarActor);
}

void UInventoryComponent::EquipItem(int32 SlotIndex)
{
	if (SlotIndex >= MaxInventorySize || ItemSlots[SlotIndex].IsEmpty()) { return; }
	
	const FItemInstance& Item = *ItemSlots[SlotIndex].Item;
	// Item.AssociatedActor->SetActorHiddenInGame(false);
	// Item.AssociatedActor->OnItemEquipped(AvatarActor);
}


void UInventoryComponent::OnSelectedItemChanged(uint8 LastSelection)
{
	UnequipItem(LastSelection);
	EquipItem(SelectedSlot);
	
	OnSelectedSlotChanged.Broadcast(LastSelection, SelectedSlot);
}
