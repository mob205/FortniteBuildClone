// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "APIInterfaceHUD.generated.h"

class UAPIWidgetBase;

UCLASS()
class DEDICATEDSERVERS_API AAPIInterfaceHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAPIWidgetBase> APITestOverlayClass;

private:
	UPROPERTY()
	TObjectPtr<UAPIWidgetBase> APITestOverlay;
};
