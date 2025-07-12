// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/APIInterfaceHUD.h"

#include "Blueprint/UserWidget.h"

void APortalHUD::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = GetOwningPlayerController();
	
	if (IsValid(PC) && SignInOverlayClass)
	{
		SignInOverlay = CreateWidget<UUserWidget>(PC, SignInOverlayClass);
		SignInOverlay->AddToViewport();

		FInputModeGameAndUI InputModeData;
		PC->SetInputMode(InputModeData);
		PC->SetShowMouseCursor(true);
	}
}

void APortalHUD::OnSignIn()
{
	if (IsValid(SignInOverlay))
	{
		SignInOverlay->RemoveFromParent();
		SignInOverlay = nullptr;
	}
	
	APlayerController* PC = GetOwningPlayerController();
	if (IsValid(PC) && DashboardOverlayClass)
	{
		DashboardOverlay = CreateWidget<UUserWidget>(PC, DashboardOverlayClass);
		DashboardOverlay->AddToViewport();
	}
}

void APortalHUD::OnSignOut()
{
	if (IsValid(DashboardOverlay))
	{
		DashboardOverlay->RemoveFromParent();
		DashboardOverlay = nullptr;
	}
	
	APlayerController* PC = GetOwningPlayerController();
	if (IsValid(PC) && SignInOverlayClass)
	{
		SignInOverlay = CreateWidget<UUserWidget>(PC, SignInOverlayClass);
		SignInOverlay->AddToViewport();
	}
}
	
