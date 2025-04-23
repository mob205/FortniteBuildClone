// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Edit/EditAbility.h"

#include "AbilitySystemComponent.h"
#include "FBCBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "AbilitySystem/Abilities/Edit/EditTargetData.h"
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
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	// End the ability if the structure is destroyed mid-edit
	SelectedStructure->OnDestroyed.AddDynamic(this, &UEditAbility::OnSelectedStructureDestroyed);
	
	TargetingActor = SpawnTargetingActor();

	// Listen for edit data
	UAbilityTask_WaitTargetData* WaitTargetData = UAbilityTask_WaitTargetData::WaitTargetDataUsingActor(
		this,
		"Wait Edit Target Data",
		EGameplayTargetingConfirmation::Custom,
		TargetingActor);
	WaitTargetData->ValidData.AddDynamic(this, &UEditAbility::OnEditDataReceived);
	WaitTargetData->ReadyForActivation();

	GetAbilitySystemComponentFromActorInfo()->CancelAbilities(&SuccessfulActivationCancelTags);

	// Rest of execution is specific to local client
	if (!IsLocallyControlled()) { return; }
	
	AddAbilityInputMappingContext();
	
	SelectedStructure->SetStructureMeshVisibility(false);

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

void UEditAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsLocallyControlled())
	{
		RemoveAbilityInputMappingContext();
		if (IsValid(TargetingActor))
		{
			TargetingActor->Destroy();
		}
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UEditAbility::OnEditDataReceived(const FGameplayAbilityTargetDataHandle& Data)
{
	if (IsValid(SelectedStructure))
	{
		SelectedStructure->SetStructureMeshVisibility(true);
		SelectedStructure->OnDestroyed.RemoveAll(this);
	}
	
	// Local only
	if (IsLocallyControlled())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		return;
	}
	
	const FEditTargetData* EditData = static_cast<const FEditTargetData*>(Data.Get(0));

	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("Server received edit data! %d"), EditData->EditBitfield));

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
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
