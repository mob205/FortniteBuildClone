// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemData.generated.h"

class UItemSlotWidget;
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
	UFUNCTION(BlueprintCallable, Category = "ItemData")
	const FText& GetItemName() const { return ItemName; }
	
	UFUNCTION(BlueprintCallable, Category = "ItemData")
	TSoftObjectPtr<UStaticMesh> GetWorldDropMesh() const { return WorldDropMesh; }

	UFUNCTION(BlueprintCallable, Category = "ItemData")
	UScriptStruct* GetItemInfoStruct() const { return ItemInstanceInfoStruct; }

	UFUNCTION(BlueprintCallable, Category = "ItemData")
	TSubclassOf<UItemSlot> GetItemSlotClass() const { return ItemSlotClass; }

	UFUNCTION(BlueprintCallable, Category = "ItemData")
	TSubclassOf<AActor> GetActorClass() const { return ActorClass.LoadSynchronous(); } // TODO: make this non-blocking

	UFUNCTION(BlueprintCallable, Category = "ItemData")
	TSubclassOf<UItemSlotWidget> GetItemSlotWidget() const { return }
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

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UItemSlotWidget> ItemSlotWidgetClass{};

	// Actor that is used when the item is equipped. This is where the item's behavior is
	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<AActor> ActorClass{};

};
