// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "APIWidgetBase.generated.h"

class UAPIData;
class UHTTPRequestManager;

UCLASS()
class DEDICATEDSERVERS_API UAPIWidgetBase : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAPIData> APIData;
};
