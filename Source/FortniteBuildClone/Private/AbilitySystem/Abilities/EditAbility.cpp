// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/EditAbility.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "AbilitySystem/Abilities/EditTargetingActor.h"
#include "Kismet/GameplayStatics.h"
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

	TargetingActor = SpawnTargetingActor();
	
	// Listen for target actor confirmation
	UAbilityTask_WaitTargetData* TargetDataTask = UAbilityTask_WaitTargetData::WaitTargetDataUsingActor(
		this,
		"Edit Ability Targeting",
		EGameplayTargetingConfirmation::UserConfirmed,
		TargetingActor);

	// Listen for input to start selecting edit tiles
	UAbilityTask_WaitGameplayEvent* StartSelectionEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, StartSelectionTag, nullptr, false, true);
	
	StartSelectionEvent->EventReceived.AddDynamic(this, &UEditAbility::StartSelection);
	StartSelectionEvent->ReadyForActivation();

	// Listen for input to stop selecting
	UAbilityTask_WaitGameplayEvent* EndSelectionEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, EndSelectionTag, nullptr, false, true);

	EndSelectionEvent->EventReceived.AddDynamic(this, &UEditAbility::EndSelection);
	EndSelectionEvent->ReadyForActivation();
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

AEditTargetingActor* UEditAbility::SpawnTargetingActor() const
{
	checkf(TargetingActors.Contains(SelectedStructure->GetStructureTag()), TEXT("EditAbility: Attempted to find targeting actor for an invalid structure tag."));
	
	TSubclassOf<AEditTargetingActor> TargetingActorClass = TargetingActors[SelectedStructure->GetStructureTag()];
	
	AEditTargetingActor* SpawnedTargetingActor = GetWorld()->SpawnActorDeferred<AEditTargetingActor>(
		TargetingActorClass,
		SelectedStructure->GetActorTransform(),
		GetAvatarActorFromActorInfo(),
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
 
	// TODO: Target actor initialization here

	UGameplayStatics::FinishSpawningActor(SpawnedTargetingActor, SelectedStructure->GetActorTransform());

	return SpawnedTargetingActor;
}

void UEditAbility::OnConfirm()
{
	EndAbility(GetCurrentAbilitySpecHandle(), CurrentActorInfo, GetCurrentActivationInfo(), false, false);
}

void UEditAbility::StartSelection(FGameplayEventData Payload)
{
	bIsSelecting = true;
}

void UEditAbility::EndSelection(FGameplayEventData Payload)
{
	bIsSelecting = false;
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
