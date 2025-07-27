// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FBCPlayerState.h"

#include "AbilitySystem/FBCAbilitySystemComponent.h"
#include "AbilitySystem/FBCAttributeSet.h"
#include "AbilitySystem/GameplayTags/FBCTags.h"
#include "Component/BuildResourceComponent.h"
#include "Component/InventoryComponent.h"

AFBCPlayerState::AFBCPlayerState()
{
	SetNetUpdateFrequency(100.f);
	
	ASC = CreateDefaultSubobject<UFBCAbilitySystemComponent>("AbilitySystemComponent");
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	ASC->GenericGameplayEventCallbacks.Add(FBCTags::InputFireDown);
	ASC->GenericGameplayEventCallbacks.Add(FBCTags::InputFireReleased);
	
	AS = CreateDefaultSubobject<UFBCAttributeSet>("AttributeSet");

	BuildResourceComp = CreateDefaultSubobject<UBuildResourceComponent>("BuildResourceComponent");

	InventoryComp = CreateDefaultSubobject<UInventoryComponent>("InventoryComponent");
}
