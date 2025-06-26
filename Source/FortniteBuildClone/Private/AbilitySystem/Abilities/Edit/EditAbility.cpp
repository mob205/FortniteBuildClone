// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/Edit/EditAbility.h"

#include "AbilitySystemComponent.h"
#include "FBCBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "AbilitySystem/Abilities/Edit/EditTargetData.h"
#include "AbilitySystem/Abilities/Edit/EditTargetingActor.h"
#include "Component/StructureGroundingComponent.h"
#include "FortniteBuildClone/FortniteBuildClone.h"
#include "Kismet/GameplayStatics.h"
#include "Structure/PlacedStructure.h"

void UEditAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	TargetingActor = nullptr;

	SelectedStructure = GetSelection<APlacedStructure>();
	if (IsValid(SelectedStructure))
	{
		HandleStructureEdit();
		return;
	}

	// Build targeting actors don't replicate to server
	if (!IsLocallyControlled()) { return; }
	
	SelectedBuildTargetingActor = GetSelection<AStructureTargetingActor>();
	if (IsValid(SelectedBuildTargetingActor))
	{
		HandleBuildTargetingActorEdit();
		return;
	}

	// No valid selection
	// Cancel ability will replicate to server
	CancelAbility(Handle, ActorInfo, ActivationInfo, true);
}

AEditTargetingActor* UEditAbility::InitializeFromStructureInfo(const FTransform& TargetTransform, const FGameplayTag& StructureTag)
{
	CurrentEditMap = &StructureInfo->GetEditMapAsset(StructureTag)->GetEditMap();

	// Get targeting information for the structure type
	FEditTargetingClassInfo EditTargetingInfo = StructureInfo->GetEditTargetingClass(StructureTag);
	TSubclassOf<AEditTargetingActor> TargetingActorClass = EditTargetingInfo.TargetingActorClass;
	bAllowRotations = EditTargetingInfo.bCanRotate;
	
	if (!TargetingActorClass)
	{
		UE_LOG(LogFBC, Warning, TEXT("UEditAbility: No edit targeting actor found for structure tag %s"), *StructureTag.ToString());
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
		return nullptr;
	}

	if (IsLocallyControlled())
	{
		FTimerDelegate TimerDelegate = FTimerDelegate::CreateUObject(this, &UEditAbility::CheckCancelRange);
		GetWorld()->GetTimerManager().SetTimer(CancelRangeTimer, TimerDelegate, CancelRangeCheckDelay, true);
	}
	
	return SpawnTargetingActor(TargetTransform, TargetingActorClass);
}

void UEditAbility::HandleStructureEdit()
{
	// End the ability if the structure is destroyed mid-edit
	SelectedStructure->OnDestroyed.AddDynamic(this, &UEditAbility::OnSelectedStructureDestroyed);

	TargetingActor = InitializeFromStructureInfo(SelectedStructure->GetActorTransform(), SelectedStructure->GetStructureTag());

	if (!TargetingActor) { return; }

	TargetingActor->SetSelectedEdit(SelectedStructure->GetEditBitfield());
	
	// Listen for edit data
	UAbilityTask_WaitTargetData* WaitTargetData = UAbilityTask_WaitTargetData::WaitTargetDataUsingActor(
		this,
		"Wait Edit Target Data",
		EGameplayTargetingConfirmation::Custom,
		TargetingActor);
	WaitTargetData->ValidData.AddDynamic(this, &UEditAbility::OnStructureEditDataReceived);
	WaitTargetData->ReadyForActivation();

	GetAbilitySystemComponentFromActorInfo()->CancelAbilities(&SuccessfulActivationCancelTags);

	// Rest of execution is specific to local client
	if (!IsLocallyControlled()) { return; }
	
	AddAbilityInputMappingContext();
	
	SelectedStructure->SetStructureMeshVisibility(false);

	ListenForInput();
}

void UEditAbility::HandleBuildTargetingActorEdit()
{
	// Editing build targeting actor is local only
	if (!IsLocallyControlled()) { return; }

	FGameplayTag StructureTag = SelectedBuildTargetingActor->GetStructureTag();
	
	CurrentEditMap = &StructureInfo->GetEditMapAsset(StructureTag)->GetEditMap();

	TargetingActor = InitializeFromStructureInfo(SelectedBuildTargetingActor->GetActorTransform(), StructureTag);

	if (!TargetingActor) { return; }

	TargetingActor->SetSelectedEdit(SelectedBuildTargetingActor->GetStructureEdit());
	
	// Listen for edit data
	UAbilityTask_WaitTargetData* WaitTargetData = UAbilityTask_WaitTargetData::WaitTargetDataUsingActor(
		this,
		"Wait Edit Target Data",
		EGameplayTargetingConfirmation::Custom,
		TargetingActor);
	WaitTargetData->ValidData.AddDynamic(this, &UEditAbility::OnBuildTargetingEditDataReceived);
	WaitTargetData->ReadyForActivation();
	
	AddAbilityInputMappingContext();

	// Toggle the edit target status here instead of in the build ability since it turns off the actor's response to visibility
	// If this was in build ability, edit wouldn't be able to select it
	SelectedBuildTargetingActor->ToggleEditTarget(true);

	ListenForInput();
}

AEditTargetingActor* UEditAbility::SpawnTargetingActor(const FTransform& SpawnTransform, TSubclassOf<AEditTargetingActor> ActorClass) const
{
	AEditTargetingActor* SpawnedTargetingActor = GetWorld()->SpawnActorDeferred<AEditTargetingActor>(
		ActorClass,
		SpawnTransform,
		GetAvatarActorFromActorInfo(),
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);


	APlayerController* AvatarPC = Cast<APlayerController>(Cast<APawn>(GetAvatarActorFromActorInfo())->GetController());
	SpawnedTargetingActor->InitializeEditTargeting(AvatarPC, Range, CurrentEditMap);
	
	UGameplayStatics::FinishSpawningActor(SpawnedTargetingActor, SpawnTransform);

	return SpawnedTargetingActor;
}

void UEditAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsValid(SelectedStructure))
	{
		SelectedStructure->SetStructureMeshVisibility(true);
		SelectedStructure->OnDestroyed.RemoveAll(this);
		SelectedStructure = nullptr;
	}

	if (IsValid(SelectedBuildTargetingActor))
	{
		SelectedBuildTargetingActor->ToggleEditTarget(false);
		SelectedBuildTargetingActor = nullptr;
	}

	
	if (IsLocallyControlled())
	{
		GetWorld()->GetTimerManager().ClearTimer(CancelRangeTimer);
		RemoveAbilityInputMappingContext();
	}
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UEditAbility::OnStructureEditDataReceived(const FGameplayAbilityTargetDataHandle& Data)
{
	if (HasAuthority(&CurrentActivationInfo))
	{
		const FEditTargetData* EditData = static_cast<const FEditTargetData*>(Data.Get(0));
		int32 EditBitfield = EditData->EditBitfield;
		EditStructure(EditBitfield, 90 * EditData->YawCWTurns);
	}
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UEditAbility::OnBuildTargetingEditDataReceived(const FGameplayAbilityTargetDataHandle& Data)
{
	const FEditTargetData* EditData = static_cast<const FEditTargetData*>(Data.Get(0));

	// Invalid edit
	if (!CurrentEditMap->Contains(EditData->EditBitfield)) { return; }
		
	int AvatarNumCWTurns = UFBCBlueprintLibrary::SnapAngleToGridInt(GetAvatarActorFromActorInfo()->GetActorRotation().Yaw) / 90;
	
	SelectedBuildTargetingActor->SetStructureEdit(EditData->EditBitfield);

	// Set the rotation offset so the build targeting actor appears in the exact same orientation as the edit targeting actor
	SelectedBuildTargetingActor->SetRotationOffset(EditData->YawCWTurns - AvatarNumCWTurns);
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
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
	UStructureGroundingComponent* SelectedGroundingComponent = SelectedStructure->GetGroundingComponent();
	const TSet<UStructureGroundingComponent*> OldNeighbors = SelectedGroundingComponent->GetNeighbors(); 
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

	UStructureGroundingComponent* SpawnedGroundingComp = SpawnedStructure->GetGroundingComponent();
	SpawnedGroundingComp->NotifyGroundUpdate();

	const TSet<UStructureGroundingComponent*>& NewNeighbors = SpawnedGroundingComp->GetNeighbors();
	for (const auto& Neighbor : OldNeighbors)
	{
		if (!NewNeighbors.Contains(Neighbor))
		{
			Neighbor->RemoveNeighbor(SelectedGroundingComponent);
		}
	}
}

void UEditAbility::CheckCancelRange()
{
	FVector AvatarLocation = GetAvatarActorFromActorInfo()->GetActorLocation();
	FVector TargetingActorLocation = TargetingActor->GetActorLocation();

	float SqrDisplacement = (AvatarLocation - TargetingActorLocation).SquaredLength();
	if (SqrDisplacement > CancelRange * CancelRange)
	{
		CancelAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), true);
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

void UEditAbility::ListenForInput()
{
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