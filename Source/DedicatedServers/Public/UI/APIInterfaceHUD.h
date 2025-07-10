// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Interfaces/ManagerHUD.h"
#include "APIInterfaceHUD.generated.h"

class UAPIWidgetBase;

UCLASS()
class DEDICATEDSERVERS_API APortalHUD : public AHUD, public IManagerHUD
{
	GENERATED_BODY()

public:
	// IHudManagement
	UFUNCTION(BlueprintCallable)
	virtual void OnSignIn() override;
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAPIWidgetBase> SignInOverlayClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAPIWidgetBase> DashboardOverlayClass;

	
private:
	UPROPERTY()
	TObjectPtr<UAPIWidgetBase> SignInOverlay;

	UPROPERTY()
	TObjectPtr<UAPIWidgetBase> DashboardOverlay;
};
