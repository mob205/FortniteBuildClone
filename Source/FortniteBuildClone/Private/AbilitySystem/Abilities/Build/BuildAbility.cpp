// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Build/BuildAbility.h"

#include "AbilitySystem/Abilities/Build/StructureTargetingActor.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"
#include "Structure/PlacedStructure.h"
#include "Data/StructureInfoDataAsset.h"
#include "Kismet/GameplayStatics.h"
#include "FBCBlueprintLibrary.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/Abilities/Build/BuildTargetData.h"
#include "Structure/PlacementStrategy/PlacementStrategy.h"
#include "FortniteBuildClone/FortniteBuildClone.h"

UBuildAbility::UBuildAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

UGameplayEffect* UBuildAbility::GetCostGameplayEffect() const
{
	if (MaterialCostEffects.Contains(CurrentMaterialType))
	{
		return MaterialCostEffects[CurrentMaterialType]->GetDefaultObject<UGameplayEffect>();
	}
	else
	{
		return nullptr;
	}
}

void UBuildAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* Avatar = GetAvatarActorFromActorInfo();

	StrategyWorldSubsystem = GetWorld()->GetSubsystem<UStructureStrategyWorldSubsystem>();

	AddAbilityInputMappingContext();
	
	// Spawn targeting actor
	TargetingActor = GetWorld()->SpawnActorDeferred<AStructureTargetingActor>(TargetingActorClass,
		Avatar->GetActorTransform(),
		Avatar,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

	FGameplayAbilityTargetingLocationInfo StartLocation{};
	StartLocation.SourceActor = Avatar;
	TargetingActor->StartLocation = StartLocation;
	TargetingActor->SetAvatar(Cast<APawn>(Avatar));

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

void UBuildAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	RemoveAbilityInputMappingContext();
}

void UBuildAbility::PlaceStructure(const FGameplayAbilityTargetDataHandle& Data)
{
	// Only the server should place structures
	if (!HasAuthority(&CurrentActivationInfo)) { return; }

	const FBuildTargetData* BuildData = static_cast<const FBuildTargetData*>(Data.Get(0));

	CurrentMaterialType = BuildData->MaterialType;
	if (!CommitAbility(GetCurrentAbilitySpecHandle(), CurrentActorInfo, GetCurrentActivationInfo()))
	{
		UE_LOG(LogFBC, Warning, TEXT("BuildAbility: Insufficient materials"));
		return;
	}
	
	FTransform BuildingTransform{BuildData->Rotation.Quaternion(), BuildData->Location};
	BuildingTransform = UFBCBlueprintLibrary::SnapTransformToGrid(BuildingTransform);

	UPlacementStrategy* PlacementStrategy = StrategyWorldSubsystem->GetStrategy(BuildData->StructureTag);
	if (!IsValid(PlacementStrategy))
	{
		UE_LOG(LogFBC, Error, TEXT("BuildAbility: No valid placement strategy found."));
	}
	
	if (!PlacementStrategy->CanPlace(BuildingTransform))
	{
		UE_LOG(LogFBC, Warning, TEXT("BuildAbility: Requested placement is invalid (not supported by structures or ground)"));
	}

	// Validate the requested build
	// Could also check for range using placement strategy's TargetingRange
	// (e.g. make sure player isn't trying to build across the map)
	if (PlacementStrategy->IsOccupied(BuildingTransform))
	{
		UE_LOG(LogFBC, Warning, TEXT("BuildAbility: Request placement is in an occupied location."))
		return;
	}
	
	// Build request validated
	
	// Get class of actor to spawn
	TSubclassOf<APlacedStructure> StructureActorClass = StructureInfo->GetStructureActorClass(BuildData->StructureTag);

	if (!IsValid(StructureActorClass))
	{
		UE_LOG(LogFBC, Error,
			TEXT("BuildAbility: No valid structure classes found for structure tag %s. Could not spawn structure."),
			*BuildData->StructureTag.GetTagName().ToString());
		return;
	}

	// Spawn and initialize structure
	APlacedStructure* PlacedStructure = GetWorld()->SpawnActorDeferred<APlacedStructure>(StructureActorClass, BuildingTransform);

	
	
	UGameplayStatics::FinishSpawningActor(PlacedStructure, BuildingTransform);
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
	FGameplayTag Tag = Payload.EventTag;

	if (IsValid(TargetingActor))
	{
		TargetingActor->SetGhostMesh(StructureInfo->GetMesh(Tag));
		TargetingActor->SetPlacementStrategy(StrategyWorldSubsystem->GetStrategy(Tag));
		TargetingActor->SetStructureTag(Tag);
	}
}