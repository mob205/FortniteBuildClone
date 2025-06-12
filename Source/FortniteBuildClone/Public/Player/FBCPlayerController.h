// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FBCPlayerController.generated.h"

class UFBCHUDWidget;
class IInteractable;
class UInputMappingContext;
/**
 * 
 */
UCLASS()
class FORTNITEBUILDCLONE_API AFBCPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void OnRep_PlayerState() override;

	virtual void OnPossess(APawn* InPawn) override;
	
	virtual void SetupInputComponent() override;

	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void SetInteracting(bool bInIsInteracting);
	
private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> InputContext;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UFBCHUDWidget> HUDWidgetClass{};

	// Interaction 
	UPROPERTY(EditDefaultsOnly, Category = "Interact")
	float InteractRange{768.f};

	UPROPERTY()
	TObjectPtr<AActor> SelectedInteractable;

	UFUNCTION(Server, Reliable)
	void ServerStartInteract(AActor* Interactable);

	UFUNCTION(Server, Reliable)
	void ServerStopInteract(AActor* Interactable);
	
	bool bIsInteracting{};

	AActor* GetInteractable() const;

	void StartInteraction(AActor* Interactable);
	void StopInteraction(AActor* Interactable);
	// End interaction

	void InitializeHUD();
};