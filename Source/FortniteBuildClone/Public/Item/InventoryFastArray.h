// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemInstance.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "UObject/Object.h"
#include "InventoryFastArray.generated.h"

USTRUCT(BlueprintType)
struct FInventorySerializerItem : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	FItemInstance ItemInstance;

	UPROPERTY()
	int32 SlotIndex;
};

DECLARE_DELEGATE_ThreeParams(FOnItemAddedSignature, const FItemInstance&, int32, int32);
DECLARE_DELEGATE_ThreeParams(FOnItemRemovedSignature, const FItemInstance&, int32, int32);
DECLARE_DELEGATE_ThreeParams(FOnItemChangedSignature, const FItemInstance&, int32, int32);

USTRUCT(BlueprintType)
struct FORTNITEBUILDCLONE_API FInventoryFastArray : public FFastArraySerializer
{
	GENERATED_BODY()

	FOnItemRemovedSignature OnItemRemoved;
	FOnItemAddedSignature OnItemAdded;
	FOnItemChangedSignature OnItemChanged;

	int32 AddItem(const FItemInstance& ItemInstance, int32 SlotIndex);
	void RemoveItem(int32 SlotIndex);
	const TArray<FInventorySerializerItem>& GetItems() const { return Items; }
	
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
