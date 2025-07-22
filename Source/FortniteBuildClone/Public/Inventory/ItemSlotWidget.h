// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemSlotWidget.generated.h"

class AEquippedItemActor;
struct FItemInstance;
/**
 * 
 */
UCLASS()
class FORTNITEBUILDCLONE_API UItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void UpdateSlot(const AEquippedItemActor* Item);
};
