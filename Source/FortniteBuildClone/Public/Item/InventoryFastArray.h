// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemSlot.h"
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
	AActor* ItemActor;

	UPROPERTY()
	int32 SlotIndex;
};

DECLARE_DELEGATE_ThreeParams(FOnItemRemovedSignature, const FItemInstance&, AActor*, int32);
DECLARE_DELEGATE_ThreeParams(FOnItemAddedSignature, const FItemInstance&, AActor*, int32);
DECLARE_DELEGATE_ThreeParams(FOnItemChangedSignature, const FItemInstance&, AActor*, int32);

USTRUCT(BlueprintType)
struct FORTNITEBUILDCLONE_API FInventoryFastArray : public FFastArraySerializer
{
	GENERATED_BODY()

	FOnItemRemovedSignature OnItemRemoved;
	FOnItemAddedSignature OnItemAdded;
	FOnItemChangedSignature OnItemChanged;

	void AddItem(const FItemInstance& ItemInstance, AActor* ItemActor, int32 SlotIndex);
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
