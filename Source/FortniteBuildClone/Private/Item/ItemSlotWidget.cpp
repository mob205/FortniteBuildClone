// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ItemSlotWidget.h"

void UItemSlotWidget::UpdateSlot(const FItemInstance& InItemInstance)
{
	OnSlotUpdated(InItemInstance);
}
