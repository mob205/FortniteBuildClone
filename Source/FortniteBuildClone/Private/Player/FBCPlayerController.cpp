// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/FBCPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Interface/Interactable.h"

void AFBCPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	using UInputSubsystem = UEnhancedInputLocalPlayerSubsystem;
	if (UInputSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UInputSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(InputContext, 0);
	}
}

void AFBCPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TScriptInterface<IInteractable> CurrentInteractable = GetInteractable();
	
	// We're still interacting with the same object
	if (bIsInteracting && CurrentInteractable.GetObject() && CurrentInteractable == SelectedInteractable)
	{
		IInteractable::Execute_ContinueInteract(SelectedInteractable.GetObject(), this);
	}
	
	// We're still interacting, but we looked away to something else
	else if (bIsInteracting && SelectedInteractable.GetObject() != nullptr)
	{
		IInteractable::Execute_StopInteract(SelectedInteractable.GetObject(), this);
		SelectedInteractable = nullptr;
	}

	if (!bIsInteracting)
	{
		SelectedInteractable = CurrentInteractable;
	}
}

void AFBCPlayerController::SetInteracting(bool bInIsInteracting)
{
	bIsInteracting = bInIsInteracting;

	if (!SelectedInteractable.GetObject()) { return; }

	// Started interacting with an interactable selected
	if (bIsInteracting)
	{
		IInteractable::Execute_StartInteract(SelectedInteractable.GetObject(), this);
	}

	// Stopped interacting with an interactable selected
	else
	{
		IInteractable::Execute_StopInteract(SelectedInteractable.GetObject(), this);
	}
}

TScriptInterface<IInteractable> AFBCPlayerController::GetInteractable() const
{
	FVector StartLocation{};
	FRotator Rotation{};

	GetPlayerViewPoint(StartLocation, Rotation);

	FVector EndLocation = Rotation.Vector() * InteractRange + StartLocation;
	
	FHitResult Hit{};
	if (GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility))
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor->Implements<UInteractable>())
		{
			TScriptInterface<IInteractable> Interactable{};
			Interactable.SetObject(HitActor);
			return Interactable;
		}
	}
	return nullptr;
}
