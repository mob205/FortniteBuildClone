// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FBCPlayerCharacter.h"

#include "AbilitySystem/FBCAbilitySystemComponent.h"
#include "AbilitySystem/GameplayTags/FBCTags.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Player/FBCPlayerController.h"
#include "Player/FBCPlayerState.h"

AFBCPlayerCharacter::AFBCPlayerCharacter()
{
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArmComponent->SetupAttachment(GetRootComponent());

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera");
	CameraComponent->SetupAttachment(SpringArmComponent);
}

// Called on clients only
void AFBCPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilityActorInfo();
}

void AFBCPlayerCharacter::InitAbilityActorInfo()
{
	FBCPlayerState = GetPlayerState<AFBCPlayerState>();
	check(FBCPlayerState);

	ASC = FBCPlayerState->GetAbilitySystemComponent();
	ASC->InitAbilityActorInfo(FBCPlayerState, this);
	
	AS = FBCPlayerState->GetAttributeSet();

	PlayerController = Cast<AFBCPlayerController>(GetController());
	
	ASC->RegisterGameplayTagEvent(FBCTags::AimingDownSights).AddUObject(this, &ThisClass::HandleADS);

	Super::InitAbilityActorInfo();
}


void AFBCPlayerCharacter::HandleADS(FGameplayTag GameplayTag, int Count)
{
#if !UE_SERVER
	SetCameraADS(Count != 0);
#endif
}