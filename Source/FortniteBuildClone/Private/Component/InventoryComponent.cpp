// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/InventoryComponent.h"

#include "Item/ItemData.h"
#include "Kismet/KismetSystemLibrary.h"
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
		if (ItemSlots[i] == nullptr)
		{
			return i;
		}
	}
	return -1;
}

void UInventoryComponent::TryAddItem_Implementation(FItemInstance Item)
{
	if (!CanAddItem()) { return; }

	AActor* ItemActor = GetWorld()->SpawnActor(Item.ItemData->GetActorClass());
	Inventory.AddItem(Item, ItemActor, GetAvailableSlotIndex());
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, Inventory);
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	Inventory.OnItemRemoved.BindUObject(this, &ThisClass::OnItemRemoved);
	Inventory.OnItemAdded.BindUObject(this, &ThisClass::OnItemAdded);
	Inventory.OnItemAdded.BindUObject(this, &ThisClass::OnItemChanged);
	
	ItemSlots.Init(nullptr, MaxInventorySize);
}

void UInventoryComponent::OnItemRemoved(const FItemInstance& Item, AActor* Actor, int32 SlotIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red,
		FString::Printf(TEXT("%d removed item %s %s %d"),
			UKismetSystemLibrary::IsServer(this),
			*Item.ItemData->GetItemName().ToString(),
			*Actor->GetName(),
			SlotIndex));
}

void UInventoryComponent::OnItemAdded(const FItemInstance& Item, AActor* Actor, int32 SlotIndex)
{
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green,
		FString::Printf(TEXT("%d added item %s %s %d"),
			UKismetSystemLibrary::IsServer(this),
			*Item.ItemData->GetItemName().ToString(),
			*Actor->GetName(),
			SlotIndex));
}

void UInventoryComponent::OnItemChanged(const FItemInstance& Item, AActor* Actor, int32 SlotIndex)
{
	if (Item.ItemData == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("Null item data")));
	}
	if (Actor == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("Null actor")));
	}
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Green,
		FString::Printf(TEXT("%d changed item %s %s %d"),
			UKismetSystemLibrary::IsServer(this),
			*Item.ItemData->GetItemName().ToString(),
			*Actor->GetName(),
			SlotIndex));
}

void UInventoryComponent::UpdateSlots()
{
	for (const auto& Item : Inventory.GetItems())
	{
		const UItemSlot* Slot = ItemSlots[Item.SlotIndex];
		const UClass* CurrentSlotClass = Slot ? Slot->GetClass() : nullptr;
		const UClass* ItemSlotClass = Item.ItemInstance.ItemData->GetClass();
		
		if (CurrentSlotClass != ItemSlotClass)
		{
			if (Slot)
			{
				RemoveSlot(Item.SlotIndex);
			}
			if (ItemSlotClass)
			{
				AddSlot(Item.SlotIndex, ItemSlotClass, Item.ItemInstance);
			}
		}
	}
}

void UInventoryComponent::RemoveSlot(int32 SlotIndex)
{
	ItemSlots[SlotIndex]->MarkAsGarbage();
	ItemSlots[SlotIndex] = nullptr;
	OnSlotRemoved.Broadcast(SlotIndex);
}

void UInventoryComponent::AddSlot(int32 SlotIndex, const UClass* SlotClass, const FItemInstance& Item)
{
	UItemSlot* Slot = NewObject<UItemSlot>(this, SlotClass);
	ItemSlots[SlotIndex] = Slot;
	Slot->Initialize(Item);
}





