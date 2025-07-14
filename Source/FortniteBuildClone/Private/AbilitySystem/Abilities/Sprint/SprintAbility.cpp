// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Sprint/SprintAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/FBCAttributeSet.h"
#include "Component/FBCCharacterMovementComponent.h"

void USprintAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

	// Listen for stamina changes to check if it reaches 0
	ASC->GetGameplayAttributeValueChangeDelegate(UFBCAttributeSet::GetStaminaAttribute()).AddUObject(this, &USprintAbility::CheckStamina);

	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	FGameplayEffectSpecHandle StartEffectHandle = ASC->MakeOutgoingSpec(StartEffectClass, 1, EffectContext);
	ASC->ApplyGameplayEffectSpecToSelf(*StartEffectHandle.Data);

	CMC = GetAvatarActorFromActorInfo()->GetComponentByClass<UFBCCharacterMovementComponent>();
	
	// TODO: If multiple CMCs are going to be used, this can be made into an interface or similar
	check(IsValid(CMC));

	SetSprintStatus(true);
}

void USprintAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	// Unsubscribe from stamina delegate
	GetAbilitySystemComponentFromActorInfo()->GetGameplayAttributeValueChangeDelegate(UFBCAttributeSet::GetStaminaAttribute()).RemoveAll(this);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	
	FGameplayEffectContextHandle EffectContext = ASC->MakeEffectContext();
	FGameplayEffectSpecHandle EndEffectHandle = ASC->MakeOutgoingSpec(EndEffectClass, 1, EffectContext);
	ASC->ApplyGameplayEffectSpecToSelf(*EndEffectHandle.Data);

	SetSprintStatus(false);
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USprintAbility::CheckStamina(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue <= 0)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void USprintAbility::SetSprintStatus(bool bSprinting) const
{
	if (IsValid(CMC))
	{
		// Let the CMC handle sprinting the intent to sprint, which is client -> server only
		if (IsPredictingClient())
		{
			CMC->ToggleWantsToSprint(bSprinting);
		}
		
		// Server still maintains authority in whether sprinting is allowed 
		CMC->ToggleCanSprint(bSprinting);
	}
}

