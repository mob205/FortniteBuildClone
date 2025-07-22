// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/FBCAttributeSet.h"

#include "Net/UnrealNetwork.h"

TMap<EFBCResourceType, FGameplayAttribute> UFBCAttributeSet::ResourceToAttributeMap{};

UFBCAttributeSet::UFBCAttributeSet()
{
	ResourceToAttributeMap.Add(EFBCResourceType::FBCMat_Wood, GetWoodAttribute());
	ResourceToAttributeMap.Add(EFBCResourceType::FBCMat_Brick, GetBrickAttribute());
	ResourceToAttributeMap.Add(EFBCResourceType::FBCMat_Metal, GetMetalAttribute());
}

void UFBCAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UFBCAttributeSet, Wood, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFBCAttributeSet, Brick, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFBCAttributeSet, Metal, COND_None, REPNOTIFY_Always);
	
	DOREPLIFETIME_CONDITION_NOTIFY(UFBCAttributeSet, Stamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFBCAttributeSet, MaxStamina, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFBCAttributeSet, Shields, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFBCAttributeSet, Health, COND_None, REPNOTIFY_Always);
}

void UFBCAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	if (Attribute == GetStaminaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0, GetMaxStamina());
	}
}
