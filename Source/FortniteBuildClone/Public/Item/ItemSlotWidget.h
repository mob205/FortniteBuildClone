// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemSlotWidget.generated.h"

class UItemSlot;
/**
 * 
 */
UCLASS()
class FORTNITEBUILDCLONE_API UItemSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetSlot(UItemSlot* InItemSlot);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnSlotSet();

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<UItemSlot> ItemSlot;
};
