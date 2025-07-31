// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/General/EquippedItemActor.h"
#include "Engine/DataAsset.h"
#include "ItemData.generated.h"

class UCrosshairWidgetBase;
class UItemSlotWidget;

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
	TSubclassOf<AEquippedItemActor> GetActorClass() const { return ActorClass.LoadSynchronous(); } // TODO: make this non-blocking

	UFUNCTION(BlueprintCallable, Category = "ItemData")
	TSubclassOf<UItemSlotWidget> GetItemSlotWidgetClass() const { return ItemSlotWidgetClass; }

	UFUNCTION(BlueprintCallable, Category = "ItemData")
	FLinearColor GetItemColor() const { return ItemColor; }

	UFUNCTION(BlueprintCallable, Category = "ItemData")
	TSoftObjectPtr<UTexture> GetItemSlotImage() const { return ItemSlotImage; }

	UFUNCTION(BlueprintCallable, Category = "ItemData")
	int32 GetMaxStackSize() const { return MaxStackSize; }

	UFUNCTION(BlueprintCallable, Category = "ItemData")
	const FName& GetSocketName() const { return SocketName; }

	UFUNCTION(BlueprintCallable, Category = "ItemData")
	const TArray<TSubclassOf<UGameplayAbility>>& GetGrantedAbilities() const { return GrantedAbilities; }

	UFUNCTION(BlueprintCallable, Category = "ItemData")
	UAnimMontage* GetEquipMontage() const { return EquipMontage; }

	UFUNCTION(BlueprintCallable, Category = "ItemData")
	TSubclassOf<UCrosshairWidgetBase> GetCrosshairClass() const { return CrosshairWidgetClass; }
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	FText ItemName{};

	// Actor that is used when the item is equipped. This is where the item's behavior is
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	TSoftClassPtr<AEquippedItemActor> ActorClass{};

	UPROPERTY(EditDefaultsOnly, Category = "Abilities")
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities{};

	UPROPERTY(EditDefaultsOnly, Category = "Equipping")
	TObjectPtr<UAnimMontage> EquipMontage{};

	UPROPERTY(EditDefaultsOnly, Category = "Equipping")
	FName SocketName{"RightHand"};
	
	// Mesh to use when item is in the world (not in an inventory)
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	TSoftObjectPtr<UStaticMesh> WorldDropMesh{};

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSoftObjectPtr<UTexture> ItemSlotImage{};

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	FLinearColor ItemColor{};
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UItemSlotWidget> ItemSlotWidgetClass{};

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UCrosshairWidgetBase> CrosshairWidgetClass{};
	
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	uint8 MaxStackSize{1};
};
