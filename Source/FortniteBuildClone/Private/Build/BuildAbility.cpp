// Fill out your copyright notice in the Description page of Project Settings.

#include "Build/BuildAbility.h"

#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Build/StructureTargetingActor.h"
#include "Build/PlacedStructure.h"
#include "Data/StructureInfoDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "GridWorldSubsystem.h"
#include "FBCBlueprintLibrary.h"

UBuildAbility::UBuildAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UBuildAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* Avatar = GetAvatarActorFromActorInfo();

	GridWorldSubsystem = GetWorld()->GetSubsystem<UGridWorldSubsystem>();
	
	// Spawn targeting actor
	TargetingActor = GetWorld()->SpawnActorDeferred<AStructureTargetingActor>(TargetingActorClass,
		Avatar->GetActorTransform(),
		Avatar,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	FGameplayAbilityTargetingLocationInfo StartLocation{};
	StartLocation.SourceActor = Avatar;
	TargetingActor->StartLocation = StartLocation;

	UGameplayStatics::FinishSpawningActor(TargetingActor, Avatar->GetActorTransform());

	// Listen for target actor confirmation
	UAbilityTask_WaitTargetData* TargetDataTask = UAbilityTask_WaitTargetData::WaitTargetDataUsingActor(
		this,
		"Build Ability Targeting",
		EGameplayTargetingConfirmation::CustomMulti,
		TargetingActor);

	TargetDataTask->ValidData.AddDynamic(this, &UBuildAbility::PlaceStructure);
	TargetDataTask->Cancelled.AddDynamic(this, &UBuildAbility::CallEndAbility);
	TargetDataTask->ReadyForActivation();

	// Listen for requests to rotate the targeting actor
	FGameplayTag RotationTag = FGameplayTag::RequestGameplayTag("Abilities.Build.Rotate");
	UAbilityTask_WaitGameplayEvent* RotationGameplayEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, RotationTag, nullptr, false, true);
	
	RotationGameplayEvent->EventReceived.AddDynamic(this, &UBuildAbility::RotateTargetingActor);
	RotationGameplayEvent->ReadyForActivation();

	// Listen for requests to set or change the structure type
	FGameplayTag ParentStructureTag = FGameplayTag::RequestGameplayTag("Abilities.Build.Structure");
	UAbilityTask_WaitGameplayEvent* SelectStructureGameplayEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this, ParentStructureTag, nullptr, false, false);
	
	SelectStructureGameplayEvent->EventReceived.AddDynamic(this, &UBuildAbility::OnSelectStructure);
	SelectStructureGameplayEvent->ReadyForActivation();
	
}

void UBuildAbility::PlaceStructure(const FGameplayAbilityTargetDataHandle& Data)
{
	// Only the server should place structures
	if (!UKismetSystemLibrary::IsServer(this)) { return; }
	
	FTransform BuildingTransform = Data.Data[0]->GetEndPointTransform();

	// Verify that there isn't another structure already taking up the space
	if (GridWorldSubsystem->IsOccupied(BuildingTransform, SelectedStructureTag))
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempted to place at already occupied spot."))
		return;
	}
	
	BuildingTransform = UFBCBlueprintLibrary::SnapTransformToGrid(BuildingTransform);

	// Get class of actor to spawn
	TSubclassOf<APlacedStructure> StructureActorClass = StructureInfo->GetStructureActorClass(SelectedStructureTag);

	if (!IsValid(StructureActorClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid structure classes found for structure tag %s"),
			*SelectedStructureTag.GetTagName().ToString());
		return;
	}

	// Spawn and register structure
	APlacedStructure* PlacedStructure = GetWorld()->SpawnActor<APlacedStructure>(StructureActorClass,
		BuildingTransform.GetLocation(), BuildingTransform.Rotator());
		
	PlacedStructure->SetStructureTag(SelectedStructureTag);
	GridWorldSubsystem->RegisterPlacedStructure(PlacedStructure);
}

void UBuildAbility::CallEndAbility(const FGameplayAbilityTargetDataHandle& Data)
{
	EndAbility(GetCurrentAbilitySpecHandle(), CurrentActorInfo, GetCurrentActivationInfo(), true, false);
}

void UBuildAbility::RotateTargetingActor(FGameplayEventData Payload)
{
	if (IsValid(TargetingActor))
	{
		TargetingActor->AddRotationOffset();
	}
}

void UBuildAbility::OnSelectStructure(FGameplayEventData Payload)
{
	SelectedStructureTag = Payload.EventTag;

	if (IsValid(TargetingActor))
	{
		TargetingActor->SetGhostActorClass(StructureInfo->GetGhostClass(SelectedStructureTag));
		TargetingActor->SetPlacementStrategy(StructureInfo->GetPlacementStrategy(SelectedStructureTag));
	}
}
