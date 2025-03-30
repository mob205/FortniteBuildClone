// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/BuildAbility.h"

#include "FBCBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "AbilitySystem/StructureTargetingActor.h"
#include "Data/StructureInfoDataAsset.h"
#include "Kismet/GameplayStatics.h"

UBuildAbility::UBuildAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UBuildAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* Avatar = GetAvatarActorFromActorInfo();

	// Spawn targeting actor
	TargetingActor = GetWorld()->SpawnActorDeferred<AStructureTargetingActor>(TargetingActorClass,
		Avatar->GetActorTransform(),
		Avatar,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	FGameplayAbilityTargetingLocationInfo StartLocation{};
	StartLocation.SourceActor = Avatar;
	TargetingActor->StartLocation = StartLocation;
	TargetingActor->SetRange(TargetingRange);

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
	
	// Target actor should already be snapped to grid by client, but snap again just to be safe 
	BuildingTransform = UFBCBlueprintLibrary::SnapTransformToGrid(BuildingTransform);

	FStructureClasses StructureClasses{};
	if (!StructureInfo->GetStructureClasses(SelectedStructureTag, StructureClasses))
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid structure classes found for structure tag %s"), *SelectedStructureTag.GetTagName().ToString());
	}

	GetWorld()->SpawnActor(StructureClasses.StructureActorClass, &BuildingTransform);

}

void UBuildAbility::CallEndAbility(const FGameplayAbilityTargetDataHandle& Data)
{
	EndAbility(GetCurrentAbilitySpecHandle(), CurrentActorInfo, GetCurrentActivationInfo(), true, false);
}

void UBuildAbility::RotateTargetingActor(FGameplayEventData Payload)
{
	if (IsValid(TargetingActor))
	{
		// Potentially add this to function library with other grid snap functions
		TargetingActor->AddActorWorldRotation({0, 90, 0});
	}
}

void UBuildAbility::OnSelectStructure(FGameplayEventData Payload)
{
	SelectedStructureTag = Payload.EventTag;

	if (IsValid(TargetingActor))
	{
		FStructureClasses StructureClasses{};
		StructureInfo->GetStructureClasses(SelectedStructureTag, StructureClasses);
		TargetingActor->SetGhostActorClass(StructureClasses.TargetingActorClass);
		TargetingActor->RotateToFacePlayer();
	}
}
