// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FBCPlayerState.h"

#include "AbilitySystem/FBCAbilitySystemComponent.h"
#include "AbilitySystem/FBCAttributeSet.h"

AFBCPlayerState::AFBCPlayerState()
{
	SetNetUpdateFrequency(100.f);
	
	ASC = CreateDefaultSubobject<UFBCAbilitySystemComponent>("AbilitySystemComponent");
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	AS = CreateDefaultSubobject<UFBCAttributeSet>("AttributeSet");
	
}
