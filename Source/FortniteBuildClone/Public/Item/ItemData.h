// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquippedItemActor.h"
#include "Engine/DataAsset.h"
#include "ItemData.generated.h"

class UItemSlotWidget;

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
	TSubclassOf<AEquippedItemActor> GetActorClass() const { return ActorClass.LoadSynchronous(); } // TODO: make this non-blocking

	UFUNCTION(BlueprintCallable, Category = "ItemData")
	TSubclassOf<UItemSlotWidget> GetItemSlotWidgetClass() const { return ItemSlotWidgetClass; }

	UFUNCTION(BlueprintCallable, Category = "ItemData")
	FLinearColor GetItemColor() const { return ItemColor; }

	UFUNCTION(BlueprintCallable, Category = "ItemData")
	TSoftObjectPtr<UTexture> GetItemSlotImage() const { return ItemSlotImage; }
	
protected:
	UPROPERTY(EditDefaultsOnly)
	FText ItemName{};

	// Actor that is used when the item is equipped. This is where the item's behavior is
	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<AEquippedItemActor> ActorClass{};
	
	// Struct with extra info about this item
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UScriptStruct> ItemInstanceInfoStruct{};

	// Mesh to use when item is in the world (not in an inventory)
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UStaticMesh> WorldDropMesh{};

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UTexture> ItemSlotImage{};

	UPROPERTY(EditDefaultsOnly)
	FLinearColor ItemColor{};
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UItemSlotWidget> ItemSlotWidgetClass{};
};
