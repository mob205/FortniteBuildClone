// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/InventoryComponent.h"

#include "Item/ItemData.h"
#include "Net/UnrealNetwork.h"


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
		if (ItemSlots[i] == FInventorySlot{})
		{
			return i;
		}
	}
	return -1;
}

void UInventoryComponent::TryAddItem_Implementation(FInstancedStruct ItemInstanceInfo, const UItemData* ItemData)
{
	if (!CanAddItem()) { return; }

	AActor* ItemActor = GetWorld()->SpawnActor(ItemData->GetActorClass());
	int32 SlotIndex = GetAvailableSlotIndex();
	int32 InventoryIndex = Inventory.AddItem({ ItemInstanceInfo, ItemActor, ItemData }, GetAvailableSlotIndex());
	OnItemAdded({ ItemInstanceInfo, ItemActor, ItemData }, InventoryIndex, SlotIndex);
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, Inventory);
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	Inventory.OnItemRemoved.BindUObject(this, &ThisClass::OnItemRemoved);
	Inventory.OnItemAdded.BindUObject(this, &ThisClass::OnItemAdded);
	
	ItemSlots.Init({}, MaxInventorySize);
}

void UInventoryComponent::OnItemRemoved(const FItemInstance& Item, int32 InventoryIndex, int32 SlotIndex)
{
	ItemSlots[SlotIndex] = {};
	OnSlotRemoved.Broadcast(SlotIndex);
}

void UInventoryComponent::OnItemAdded(const FItemInstance& Item, int32 InventoryIndex, int32 SlotIndex)
{
	ItemSlots[SlotIndex] = { &Item, InventoryIndex };
	OnSlotAdded.Broadcast(SlotIndex, Item);
}




