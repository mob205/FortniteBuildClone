// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Edit/EditAbility.h"

#include "AbilitySystemComponent.h"
#include "FBCBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitConfirmCancel.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/Abilities/Edit/EditTargetingActor.h"
#include "FortniteBuildClone/FortniteBuildClone.h"
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
	SelectedStructure->OnDestroyed.AddDynamic(this, &UEditAbility::OnSelectedStructureDestroyed);

	TargetingActor = SpawnTargetingActor();
	if (!TargetingActor) { return; }
	
	// Listen for target actor confirmation
	UAbilityTask_WaitConfirmCancel* WaitConfirmTask = UAbilityTask_WaitConfirmCancel::WaitConfirmCancel(this);
	WaitConfirmTask->OnConfirm.AddDynamic(this, &UEditAbility::OnConfirm);
	WaitConfirmTask->ReadyForActivation();

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

AEditTargetingActor* UEditAbility::SpawnTargetingActor() const
{
	if (!TargetingActors.Contains(SelectedStructure->GetStructureTag()))
	{
		UE_LOG(LogFBC, Warning, TEXT("UEditAbility: No edit targeting actor found for structure tag %s"), *SelectedStructure->GetStructureTag().ToString());
		return nullptr;
	}
	TSubclassOf<AEditTargetingActor> TargetingActorClass = TargetingActors[SelectedStructure->GetStructureTag()];
	
	AEditTargetingActor* SpawnedTargetingActor = GetWorld()->SpawnActorDeferred<AEditTargetingActor>(
		TargetingActorClass,
		SelectedStructure->GetActorTransform(),
		GetAvatarActorFromActorInfo(),
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	SpawnedTargetingActor->SetSelectedEdit(SelectedStructure->GetEditBitfield());

	APlayerController* AvatarPC = Cast<APlayerController>(Cast<APawn>(GetAvatarActorFromActorInfo())->GetController());
	SpawnedTargetingActor->InitializeEditTargeting(AvatarPC, Range);

	UGameplayStatics::FinishSpawningActor(SpawnedTargetingActor, SelectedStructure->GetActorTransform());

	return SpawnedTargetingActor;
}

void UEditAbility::OnConfirm()
{
	TSubclassOf<APlacedStructure> StructureClass{};
	if (TargetingActor->GetSelectedEdit(StructureClass))
	{
		UE_LOG(LogFBC, Warning, TEXT("UEditAbility: Got valid edit!"));
	}
	else
	{
		UE_LOG(LogFBC, Warning, TEXT("UEditAbility: Invalid edit! :("));
	}

	EndAbility(GetCurrentAbilitySpecHandle(), CurrentActorInfo, GetCurrentActivationInfo(), false, false);
}

void UEditAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsValid(SelectedStructure))
	{
		SelectedStructure->SetStructureMeshVisibility(true);
		SelectedStructure->OnDestroyed.RemoveAll(this);
	}
	RemoveAbilityInputMappingContext();

	if (IsValid(TargetingActor))
	{
		TargetingActor->Destroy();
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UEditAbility::StartSelection(FGameplayEventData Payload)
{
	TargetingActor->SetSelection(true);
}

void UEditAbility::EndSelection(FGameplayEventData Payload)
{
	TargetingActor->SetSelection(false);
}

void UEditAbility::OnSelectedStructureDestroyed(AActor* DestroyedActor)
{
	EndAbility(GetCurrentAbilitySpecHandle(), CurrentActorInfo, GetCurrentActivationInfo(), false, false);
}

APlacedStructure* UEditAbility::GetSelectedStructure() const
{
	APawn* AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	
	check(AvatarPawn); // Currently, avatars are always pawns

	FHitResult HitResult{};
	if (!UFBCBlueprintLibrary::TraceControllerLook(AvatarPawn->GetController(), Range, HitResult))
	{
		return nullptr;
	}
	if (APlacedStructure* AsStructure = Cast<APlacedStructure>(HitResult.GetActor()))
	{
		return AsStructure;
	}
	return nullptr;
}
