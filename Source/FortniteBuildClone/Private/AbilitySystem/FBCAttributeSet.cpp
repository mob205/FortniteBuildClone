// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/FBCAttributeSet.h"

#include "GameplayEffectExtension.h"
#include "AbilitySystem/GameplayTags/FBCTags.h"
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
	
	DOREPLIFETIME_CONDITION_NOTIFY(UFBCAttributeSet, Shields, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFBCAttributeSet, MaxShields, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFBCAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UFBCAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
}

void UFBCAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);
}

void UFBCAttributeSet::PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// Clamp health and shields to max health or the maximum amount recoverable by that gameplay effect
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		float MaxRecoverableAmount = Data.EffectSpec.GetSetByCallerMagnitude(FBCTags::MaxResourceRecoverable, false, GetMaxHealth());
		SetHealth(FMath::Clamp(GetHealth(), 0.f, MaxRecoverableAmount));
	}
	else if (Data.EvaluatedData.Attribute == GetShieldsAttribute())
	{
		float MaxRecoverableAmount = Data.EffectSpec.GetSetByCallerMagnitude(FBCTags::MaxResourceRecoverable, false, GetMaxShields());
		SetShields(FMath::Clamp(GetShields(), 0.f, MaxRecoverableAmount));
	}
	else if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		float IncomingDamageValue = FMath::Max(0, GetIncomingDamage());
		float NewShields = GetShields() - IncomingDamageValue;

		if (NewShields < 0)
		{
			IncomingDamageValue = -NewShields;
			NewShields = 0;
		}
		else
		{
			IncomingDamageValue = 0;
		}
		SetShields(NewShields);

		float NewHealth = GetHealth() - IncomingDamageValue;

		if (NewHealth < 0)
		{
			// Do dying stuff here
			NewHealth = 0;
		}
		SetHealth(NewHealth);
		SetIncomingDamage(0);
	}
}
