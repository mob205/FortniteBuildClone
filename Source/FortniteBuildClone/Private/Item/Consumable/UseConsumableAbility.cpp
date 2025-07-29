// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Consumable/UseConsumableAbility.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "AbilitySystem/GameplayTags/FBCTags.h"
#include "FortniteBuildClone/FortniteBuildClone.h"
#include "GameFramework/GameStateBase.h"
#include "Item/General/CountableItem.h"
#include "Kismet/GameplayStatics.h"

UUseConsumableAbility::UUseConsumableAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UUseConsumableAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (MaxRecoverableAttribute.IsValid() && MaxRecoverableMagnitude > 0.0f
		&& GetAbilitySystemComponentFromActorInfo()->GetNumericAttribute(MaxRecoverableAttribute) >= MaxRecoverableMagnitude)
	{
		return false;
	}
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

}

void UUseConsumableAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                            const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                            const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	ACountableItem* SourceItem = Cast<ACountableItem>(GetCurrentSourceObject());

	if (!IsValid(SourceItem))
	{
		UE_LOG(LogFBC, Error, TEXT("UseConsumableAbility: Source object was not a countable item!"));
		return;
	}

	SourceItem->SetCount(SourceItem->GetCount() - 1);

	// Apply the consumable gameplay effect
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(GameplayEffectClass, 1.0f);
	if (MaxRecoverableMagnitude > 0)
	{
		Spec.Data->SetSetByCallerMagnitude(FBCTags::MaxResourceRecoverable, MaxRecoverableMagnitude);
	}
	ActiveEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data);

	if(auto Delegate = ASC->OnGameplayEffectRemoved_InfoDelegate(ActiveEffectHandle))
	{
		Delegate->AddUObject(this, &UUseConsumableAbility::OnEffectEnded);
	}
	
	// Listen to cancel the ability if the recovery maximum is reached
	if (MaxRecoverableMagnitude > 0 && MaxRecoverableAttribute.IsValid())
	{
		ASC->GetGameplayAttributeValueChangeDelegate(MaxRecoverableAttribute).AddUObject(this, &UUseConsumableAbility::OnResourceChanged);
	}
	
	// Play the montage of using the consumable
	PlayMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this, {}, UsingAnimMontage);
	PlayMontageTask->Activate();
}

void UUseConsumableAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();

	GEngine->AddOnScreenDebugMessage(-1, 10, HasAuthority(&ActivationInfo) ? FColor::Red : FColor::Green, FString::Printf(TEXT("%lf"), UGameplayStatics::GetGameState(this)->GetServerWorldTimeSeconds() - Test));

	ASC->CurrentMontageStop();

	if (MaxRecoverableAttribute.IsValid())
	{
		ASC->GetGameplayAttributeValueChangeDelegate(MaxRecoverableAttribute).RemoveAll(this);
	}

	if (HasAuthority(&ActivationInfo))
	{
		ASC->RemoveActiveGameplayEffect(ActiveEffectHandle);
	}
	
	if (ACountableItem* SourceItem = Cast<ACountableItem>(GetCurrentSourceObject()))
	{
		if (SourceItem->GetCount() == 0)
		{
			SourceItem->RemoveFromOwningInventory();
		}
	}

}

void UUseConsumableAbility::OnMontageEnded()
{
	EndAbility(GetCurrentAbilitySpecHandle(), CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UUseConsumableAbility::OnResourceChanged(const FOnAttributeChangeData& OnAttributeChangeData)
{
	if (OnAttributeChangeData.NewValue >= MaxRecoverableMagnitude)
	{
		EndAbility(GetCurrentAbilitySpecHandle(), CurrentActorInfo, CurrentActivationInfo, true, false);	
	}
}

void UUseConsumableAbility::OnEffectEnded(const FGameplayEffectRemovalInfo& GameplayEffectRemovalInfo)
{
	EndAbility(GetCurrentAbilitySpecHandle(), CurrentActorInfo, CurrentActivationInfo, true, false);
}
