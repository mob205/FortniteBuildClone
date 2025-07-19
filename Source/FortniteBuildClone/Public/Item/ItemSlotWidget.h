// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemSlotWidget.generated.h"

struct FItemInstance;
/**
 * 
 */
UCLASS()
class FORTNITEBUILDCLONE_API UItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void UpdateSlot(const FItemInstance& InItemSlot);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnSlotUpdated(const FItemInstance& InItemSlot);
};
