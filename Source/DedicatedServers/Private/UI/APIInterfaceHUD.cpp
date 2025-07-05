// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/APIInterfaceHUD.h"

#include "UI/APIWidgetBase.h"

void AAPIInterfaceHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = GetOwningPlayerController();
	
	if (IsValid(PC) && APITestOverlayClass)
	{
		APITestOverlay = CreateWidget<UAPIWidgetBase>(PC, APITestOverlayClass);
		APITestOverlay->AddToViewport();

		FInputModeGameAndUI InputModeData;
		PC->SetInputMode(InputModeData);
		PC->SetShowMouseCursor(true);
	}
}
