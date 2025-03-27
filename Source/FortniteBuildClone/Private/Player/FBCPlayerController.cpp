// Fill out your copyright notice in the Description page of Project Settings.

#include "EnhancedInputSubsystems.h"
#include "Player/FBCPlayerController.h"

void AFBCPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	using UInputSubsystem = UEnhancedInputLocalPlayerSubsystem;
	if (UInputSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UInputSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputContext, 0);
	}
}
