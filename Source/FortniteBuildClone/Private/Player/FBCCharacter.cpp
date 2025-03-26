// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FBCCharacter.h"

#include "AbilitySystem/FBCAbilitySystemComponent.h"
#include "Player/FBCPlayerState.h"

// Called on server only
void AFBCCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();
}

// Called on clients only
void AFBCCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilityActorInfo();
}

void AFBCCharacter::InitAbilityActorInfo()
{
	AFBCPlayerState* PS = GetPlayerState<AFBCPlayerState>();
	check(PS);

	ASC = PS->GetAbilitySystemComponent();
	ASC->InitAbilityActorInfo(PS, this);

	AS = PS->GetAttributeSet();
}
