// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemData.generated.h"

class UItemSlot;

USTRUCT()
struct FTestItemData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	int TestInt{};
};

UCLASS()
class FORTNITEBUILDCLONE_API UItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	const FText& GetItemName() const { return ItemName;}
	TSoftObjectPtr<UStaticMesh> GetWorldDropMesh() const { return WorldDropMesh; }
	UScriptStruct* GetItemInfoStruct() const { return ItemInstanceInfoStruct; }
	TSubclassOf<UItemSlot> GetItemSlotClass() const { return ItemSlotClass; }
	TSubclassOf<AActor> GetActorClass() const { return ActorClass.LoadSynchronous(); } // TODO: make this non-blocking
	
protected:
	UPROPERTY(EditDefaultsOnly)
	FText ItemName{};

	// Mesh to use when item is in the world (not in an inventory)
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UStaticMesh> WorldDropMesh{};

	// Struct with extra info about this item
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UScriptStruct> ItemInstanceInfoStruct{};

	// Type of item slot to use for this item
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UItemSlot> ItemSlotClass{};

	// Actor that is used when the item is equipped. This is where the item's behavior is
	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<AActor> ActorClass{};

};
