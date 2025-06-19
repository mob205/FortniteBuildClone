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
		TargetingActor = nullptr;
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}

	CurrentEditMap = &StructureInfo->GetEditMapAsset(SelectedStructure->GetStructureTag())->GetEditMap();

	// End the ability if the structure is destroyed mid-edit
	SelectedStructure->OnDestroyed.AddDynamic(this, &UEditAbility::OnSelectedStructureDestroyed);

	// Get targeting information for the structure type
	FEditTargetingClassInfo EditTargetingInfo = StructureInfo->GetEditTargetingClass(SelectedStructure->GetStructureTag());
	TSubclassOf<AEditTargetingActor> TargetingActorClass = EditTargetingInfo.TargetingActorClass;
	bAllowRotations = EditTargetingInfo.bCanRotate;
	
	if (!TargetingActorClass)
	{
		UE_LOG(LogFBC, Warning, TEXT("UEditAbility: No edit targeting actor found for structure tag %s"), *SelectedStructure->GetStructureTag().ToString());
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
		return;
	}
	
	TargetingActor = SpawnTargetingActor(TargetingActorClass);

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

	// Listen for input to reset the edit
	UAbilityTask_WaitGameplayEvent* ResetEditEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, ResetEditTag, nullptr, false, true);

	ResetEditEvent->EventReceived.AddDynamic(this, &UEditAbility::OnEditReset);
	ResetEditEvent->ReadyForActivation();
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

AEditTargetingActor* UEditAbility::SpawnTargetingActor(TSubclassOf<AEditTargetingActor> ActorClass) const
{
	
	AEditTargetingActor* SpawnedTargetingActor = GetWorld()->SpawnActorDeferred<AEditTargetingActor>(
		ActorClass,
		SelectedStructure->GetActorTransform(),
		GetAvatarActorFromActorInfo(),
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);


	APlayerController* AvatarPC = Cast<APlayerController>(Cast<APawn>(GetAvatarActorFromActorInfo())->GetController());
	SpawnedTargetingActor->InitializeEditTargeting(AvatarPC, Range, CurrentEditMap);

	SpawnedTargetingActor->SetSelectedEdit(SelectedStructure->GetEditBitfield());

	UGameplayStatics::FinishSpawningActor(SpawnedTargetingActor, SelectedStructure->GetActorTransform());

	return SpawnedTargetingActor;
}

void UEditAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsValid(SelectedStructure))
	{
		SelectedStructure->SetStructureMeshVisibility(true);
		SelectedStructure->OnDestroyed.RemoveAll(this);
	}

	if (IsValid(TargetingActor))
	{
		TargetingActor->Destroy();
	}

	if (IsLocallyControlled())
	{
		RemoveAbilityInputMappingContext();
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UEditAbility::OnEditDataReceived(const FGameplayAbilityTargetDataHandle& Data)
{
	if (HasAuthority(&CurrentActivationInfo))
	{
		const FEditTargetData* EditData = static_cast<const FEditTargetData*>(Data.Get(0));
		int32 EditBitfield = EditData->EditBitfield;
		EditStructure(EditBitfield, 90 * EditData->YawCWTurns);
	}
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UEditAbility::EditStructure(int32 EditBitfield, int Yaw) const
{
	// Invalid edit
	if (!CurrentEditMap->Contains(EditBitfield)) { return; }

	const FStructureEditInfo& CurrentStructureInfo = CurrentEditMap->FindChecked(EditBitfield);
	if (!IsValid(CurrentStructureInfo.StructureClass))
	{
		UE_LOG(LogFBC, Error, TEXT("EditAbility: Edit is in the edit map but does not have associated structure class!"));
		return;
	}

	FVector StructureLocation = SelectedStructure->GetActorLocation();
	FRotator StructureRotation = SelectedStructure->GetActorRotation();

	bool bIsSameRotation{ true };
	
	if (bAllowRotations)
	{
		int StructureYaw = UFBCBlueprintLibrary::SnapAngleToGridInt(StructureRotation.Yaw);
		
		// No edit changes
		bIsSameRotation = StructureYaw == Yaw;
		StructureRotation.Yaw = Yaw;
	}
	// No change needed
	if (SelectedStructure->GetEditBitfield() == EditBitfield && bIsSameRotation) { return; }
	
	// We have a valid edit!
	
	// Save current structure info
	// Explicitly make copy of neighbors just in case destroying the structure leaves dangling reference
	const TSet<APlacedStructure*> OldNeighbors = SelectedStructure->GetNeighbors(); 
	EFBCResourceType SelectedMaterial = SelectedStructure->GetResourceType();
	
	// Replace old structure with new structure
	SelectedStructure->Destroy();
	
	TSubclassOf<APlacedStructure> EditStructureClass = CurrentStructureInfo.StructureClass;
	APlacedStructure* SpawnedStructure = GetWorld()->SpawnActor<APlacedStructure>(
		EditStructureClass,
		StructureLocation,
		StructureRotation);

	SpawnedStructure->SetResourceType(SelectedMaterial);

	// Editing may remove support from nearby structures or leave the new structure unsupported
	SpawnedStructure->SetEditBitfield(EditBitfield);
	SpawnedStructure->NotifyGroundUpdate();

	const TSet<APlacedStructure*>& NewNeighbors = SpawnedStructure->GetNeighbors();
	for (const auto& Neighbor : OldNeighbors)
	{
		if (!NewNeighbors.Contains(Neighbor))
		{
			// Should be safe to use destroyed structure here since the address only acts as a key
			Neighbor->RemoveNeighbor(SelectedStructure);
		}
	}
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

void UEditAbility::OnEditReset(FGameplayEventData Payload)
{
	TargetingActor->ResetEdit();
}