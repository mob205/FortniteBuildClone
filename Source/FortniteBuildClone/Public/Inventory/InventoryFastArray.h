// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "UObject/Object.h"
#include "InventoryFastArray.generated.h"

class AEquippedItemActor;

USTRUCT(BlueprintType)
struct FInventorySerializerItem : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AEquippedItemActor> Item{};

	UPROPERTY()
	uint8 SlotIndex{};
};

DECLARE_DELEGATE_TwoParams(FOnItemAddedSignature,	AEquippedItemActor*, int32 /*Slot Index*/);
DECLARE_DELEGATE_TwoParams(FOnItemRemovedSignature,	AEquippedItemActor*, int32 /*Slot Index*/);
DECLARE_DELEGATE_TwoParams(FOnItemChangedSignature,	AEquippedItemActor*, int32 /*Slot Index*/);
DECLARE_DELEGATE(FPostOnItemsChangedSignature);

USTRUCT(BlueprintType)
struct FORTNITEBUILDCLONE_API FInventoryFastArray : public FFastArraySerializer
{
	GENERATED_BODY()

	FOnItemRemovedSignature OnItemRemoved;
	FOnItemAddedSignature OnItemAdded;
	FOnItemChangedSignature OnItemChanged;
	FPostOnItemsChangedSignature PostOnItemsChanged;
	
	int32 AddItem(AEquippedItemActor* Item, int32 SlotIndex);
	void RemoveItem(AEquippedItemActor* Item);
	void RemoveItem(int32 SlotIndex);
	
	const TArray<FInventorySerializerItem>& GetItems() const { return Items; }
	TArray<FInventorySerializerItem>& GetItems() { return Items; }
	
	const FInventorySerializerItem& GetItem(int32 Index) const { return Items[Index];}
	FInventorySerializerItem& GetItem(int32 Index) { return Items[Index]; }
	
	void PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FInventorySerializerItem, FInventoryFastArray>(Items, DeltaParams, *this);
	}

private:
	UPROPERTY()
	TArray<FInventorySerializerItem> Items;
};

template<>
struct TStructOpsTypeTraits<FInventoryFastArray> : public TStructOpsTypeTraitsBase2<FInventoryFastArray>
{
	enum { WithNetDeltaSerializer = true };
};
