// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "APITestHUD.generated.h"

class UAPITestOverlay;

UCLASS()
class DEDICATEDSERVERS_API AAPITestHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAPITestOverlay> APITestOverlayClass;

private:
	UPROPERTY()
	TObjectPtr<UAPITestOverlay> APITestOverlay;
};
