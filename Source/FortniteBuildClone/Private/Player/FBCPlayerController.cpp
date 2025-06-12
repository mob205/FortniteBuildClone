// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/FBCPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Interface/Interactable.h"
#include "Player/FBCPlayerState.h"
#include "UI/FBCHUDWidget.h"

void AFBCPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitializeHUD();
}

void AFBCPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	InitializeHUD();
}


void AFBCPlayerController::InitializeHUD()
{
	if (IsLocalController() && HUDWidgetClass)
	{
		AFBCPlayerState* PS = Cast<AFBCPlayerState>(PlayerState);
		check(PS);
		
		UFBCHUDWidget* HUD = CreateWidget<UFBCHUDWidget>(this, HUDWidgetClass);
		HUD->InitializeHUD(PS, PS->GetAbilitySystemComponent());
		HUD->AddToViewport();
	}
}


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

	AActor* CurrentInteractable = GetInteractable();
	
	// We're still interacting, but we looked away to something else
	if (bIsInteracting && SelectedInteractable && CurrentInteractable != SelectedInteractable)
	{
		StopInteraction(SelectedInteractable);
		
		SelectedInteractable = nullptr;
	}

	// Only change selections when we're not actively interacting
	if (!bIsInteracting)
	{
		SelectedInteractable = CurrentInteractable;
	}
}

void AFBCPlayerController::SetInteracting(bool bInIsInteracting)
{
	bIsInteracting = bInIsInteracting;

	if (!SelectedInteractable) { return; }

	// Started interacting with an interactable selected
	if (bIsInteracting)
	{
		StartInteraction(SelectedInteractable);
	}
	// Stopped interacting with an interactable selected
	else
	{
		StopInteraction(SelectedInteractable);
	}
}

AActor* AFBCPlayerController::GetInteractable() const
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
			return HitActor;
		}
	}
	return nullptr;
}

void AFBCPlayerController::ServerStartInteract_Implementation(AActor* Interactable)
{
	IInteractable::Execute_StartInteract(Interactable, GetPawn());
}
void AFBCPlayerController::ServerStopInteract_Implementation(AActor* Interactable)
{
	IInteractable::Execute_StopInteract(Interactable, GetPawn());
}

void AFBCPlayerController::StartInteraction(AActor* Interactable)
{
	if (HasAuthority())
	{
		IInteractable::Execute_StartInteract(Interactable, GetPawn());
	}
	else
	{
		ServerStartInteract(Interactable);
	}
}

void AFBCPlayerController::StopInteraction(AActor* Interactable)
{
	if (HasAuthority())
	{
		IInteractable::Execute_StopInteract(Interactable, GetPawn());
	}
	else
	{
		ServerStopInteract(Interactable);
	}
}
