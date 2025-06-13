// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/FBCAttributeSet.h"

#include "Net/UnrealNetwork.h"

void UFBCAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UFBCAttributeSet, Wood, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFBCAttributeSet, Brick, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFBCAttributeSet, Metal, COND_None, REPNOTIFY_Always);

}
