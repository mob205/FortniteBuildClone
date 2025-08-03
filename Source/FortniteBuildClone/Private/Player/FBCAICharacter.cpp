// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FBCAICharacter.h"
#include "AbilitySystem/FBCAbilitySystemComponent.h"
#include "AbilitySystem/FBCAttributeSet.h"

AFBCAICharacter::AFBCAICharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	SetNetUpdateFrequency(100.f);
	
	ASC = CreateDefaultSubobject<UFBCAbilitySystemComponent>("AbilitySystemComponent");
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AS = CreateDefaultSubobject<UFBCAttributeSet>("AttributeSet");
}

void AFBCAICharacter::InitAbilityActorInfo()
{
	ASC->InitAbilityActorInfo(this, this);
	Super::InitAbilityActorInfo();
}
