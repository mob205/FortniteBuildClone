// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/EditAbility.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "Structure/PlacedStructure.h"

void UEditAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	SelectedStructure = GetSelectedStructure();
	if (!IsValid(SelectedStructure))
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, false);
		return;
	}
	GetAbilitySystemComponentFromActorInfo()->CancelAbilities(&SuccessfulActivationCancelTags);
	AddAbilityInputMappingContext();
	
	SelectedStructure->SetStructureMeshVisibility(false);

	UAbilityTask_WaitConfirmCancel* TempWaitConfirm = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);
	TempWaitConfirm->OnConfirm.AddDynamic(this, &UEditAbility::OnConfirm);
	TempWaitConfirm->ReadyForActivation();
	
}

void UEditAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (SelectedStructure)
	{
		SelectedStructure->SetStructureMeshVisibility(true);
	}
	RemoveAbilityInputMappingContext();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UEditAbility::OnConfirm()
{
	EndAbility(GetCurrentAbilitySpecHandle(), CurrentActorInfo, GetCurrentActivationInfo(), false, false);
}

APlacedStructure* UEditAbility::GetSelectedStructure() const
{
	APawn* AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	
	check(AvatarPawn); // Currently, avatars are always pawns

	FVector ViewStart{};
	FRotator ViewRot{};
	AvatarPawn->GetController()->GetPlayerViewPoint(ViewStart, ViewRot);

	FVector ViewEnd = ViewStart + (ViewRot.Vector() * Range);
	
	FHitResult HitResult{};
	if (!GetWorld()->LineTraceSingleByChannel(HitResult, ViewStart, ViewEnd, ECollisionChannel::ECC_Visibility))
	{
		return nullptr;
	}
	if (APlacedStructure* AsStructure = Cast<APlacedStructure>(HitResult.GetActor()))
	{
		return AsStructure;
	}
	return nullptr;
}
