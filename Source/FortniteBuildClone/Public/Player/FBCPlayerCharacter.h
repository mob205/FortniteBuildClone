// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FBCCharacterBase.h"
#include "FBCPlayerCharacter.generated.h"

class AFBCPlayerController;
class AFBCPlayerState;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class FORTNITEBUILDCLONE_API AFBCPlayerCharacter : public AFBCCharacterBase
{
	GENERATED_BODY()

public:
	AFBCPlayerCharacter();
protected:
	void OnRep_PlayerState();
	virtual void InitAbilityActorInfo() override;
	void HandleADS(FGameplayTag GameplayTag, int Count);

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AFBCPlayerState> FBCPlayerState;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AFBCPlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UCameraComponent> CameraComponent;

	UFUNCTION(BlueprintImplementableEvent)
	void SetCameraADS(bool bIsAiming);
};
