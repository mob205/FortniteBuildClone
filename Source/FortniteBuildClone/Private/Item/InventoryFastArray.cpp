// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/InventoryFastArray.h"


void FInventoryFastArray::AddItem(const FItemInstance& ItemInstance, AActor* ItemActor, int32 SlotIndex)
{
	FInventorySerializerItem& NewItem = Items.AddDefaulted_GetRef();
	NewItem.ItemActor = ItemActor;
	NewItem.ItemInstance = ItemInstance;
	NewItem.SlotIndex = SlotIndex;

	MarkItemDirty(NewItem);
}

void FInventoryFastArray::RemoveItem(int32 SlotIndex)
{
	for (auto Iter = Items.CreateIterator(); Iter; ++Iter)
	{
		if (Iter->SlotIndex == SlotIndex)
		{
			Iter.RemoveCurrent();
			MarkArrayDirty();
		}
	}
}

void FInventoryFastArray::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize)
{
	for (const int32 Index : RemovedIndices)
	{
		OnItemRemoved.ExecuteIfBound(Items[Index].ItemInstance, Items[Index].ItemActor, Items[Index].SlotIndex);
	}
}

void FInventoryFastArray::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	for (const int32 Index : AddedIndices)
	{
		OnItemAdded.ExecuteIfBound(Items[Index].ItemInstance, Items[Index].ItemActor, Items[Index].SlotIndex);
	}
}

void FInventoryFastArray::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	for (const int32 Index : ChangedIndices)
	{
		OnItemChanged.ExecuteIfBound(Items[Index].ItemInstance, Items[Index].ItemActor, Items[Index].SlotIndex);
	}
}
