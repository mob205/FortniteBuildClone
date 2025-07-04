// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Test/APITestHUD.h"

#include "UI/Test/APITestOverlay.h"

void AAPITestHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = GetOwningPlayerController();
	
	if (IsValid(PC) && APITestOverlayClass)
	{
		APITestOverlay = CreateWidget<UAPITestOverlay>(PC, APITestOverlayClass);
		APITestOverlay->AddToViewport();
	}
}
