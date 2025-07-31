// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrosshairWidgetBase.generated.h"

class AEquippedItemActor;

UCLASS()
class FORTNITEBUILDCLONE_API UCrosshairWidgetBase : public UUserWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void InitializeItem(AEquippedItemActor* Item);
};
