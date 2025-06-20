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
#include "Component/BuildResourceComponent.h"
#include "Structure/PlacementStrategy/PlacementStrategy.h"
#include "FortniteBuildClone/FortniteBuildClone.h"

UBuildAbility::UBuildAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

UGameplayEffect* UBuildAbility::GetCostGameplayEffect() const
{
	// Allow the ability to start to show targeting actor even with no mats
	// Placing structures gets the cost GE while active, so materials are still needed
	if (!IsActive()) { return nullptr; }
	
	EFBCResourceType MaterialKey = EFBCResourceType::FBCMat_Max;

	// If we're local, use whatever material the resource comp has selected
	if (IsLocallyControlled() && ResourceComponent)
	{
		MaterialKey = ResourceComponent->GetCurrentResourceType();
	}

	// If we're server, then we're applying the cost after placing a building. Used the type passed in target data
	else if (HasAuthority(&CurrentActivationInfo))
	{
		MaterialKey = CachedMaterialType;
	}
	
	if (MaterialCostEffects.Contains(MaterialKey))
	{
		return MaterialCostEffects[MaterialKey]->GetDefaultObject<UGameplayEffect>();
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
	
	Avatar = GetAvatarActorFromActorInfo();

	if (AActor* OwnerActor = GetAbilitySystemComponentFromActorInfo()->GetOwnerActor())
	{
		ResourceComponent = OwnerActor->GetComponentByClass<UBuildResourceComponent>();
	}

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
	TargetingActor->SetResourceComponent(ResourceComponent);

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

	// Listen for edit ability to be activated
	GetAbilitySystemComponentFromActorInfo()->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("Abilities.Edit"))
		.AddUObject(this, &UBuildAbility::OnStartEdit);
}

void UBuildAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	RemoveAbilityInputMappingContext();
	GetAbilitySystemComponentFromActorInfo()->RegisterGameplayTagEvent(FGameplayTag::RequestGameplayTag("Abilities.Edit"))
		.RemoveAll(this);
}

void UBuildAbility::PlaceStructure(const FGameplayAbilityTargetDataHandle& Data)
{
	// Only the server should place structures
	if (!HasAuthority(&CurrentActivationInfo)) { return; }

	const FBuildTargetData* BuildData = static_cast<const FBuildTargetData*>(Data.Get(0));
	
	FTransform BuildingTransform{BuildData->Rotation.Quaternion(), BuildData->Location};
	BuildingTransform = UFBCBlueprintLibrary::SnapTransformToGrid(BuildingTransform);

	// Validate the requested build
	// Could also check for range using placement strategy's TargetingRange
	// (e.g. make sure player isn't trying to build across the map)

	// Ensure placement strategy is setup properly
	UPlacementStrategy* PlacementStrategy = StrategyWorldSubsystem->GetStrategy(BuildData->StructureTag);
	if (!IsValid(PlacementStrategy))
	{
		UE_LOG(LogFBC, Error, TEXT("BuildAbility: No valid placement strategy found."));
	}

	// Ensure the requested location is supported
	if (!PlacementStrategy->CanPlace(BuildingTransform, BuildData->Edit))
	{
		UE_LOG(LogFBC, Warning, TEXT("BuildAbility: Requested placement is invalid (not supported by structures or ground)"));
		return;
	}

	// Check nothing is already occupying that spot
	if (PlacementStrategy->IsOccupied(BuildingTransform))
	{
		UE_LOG(LogFBC, Warning, TEXT("BuildAbility: Request placement is in an occupied location."))
		return;
	}

	// Ensure we can find the class to spawn through edit data 
	const UEditMapDataAsset* EditMapDataAsset = StructureInfo->GetEditMapAsset(BuildData->StructureTag);
	if (!EditMapDataAsset)
	{
		UE_LOG(LogFBC, Error, TEXT("BuildAbility: No edit map found for structure tag %s"), *BuildData->StructureTag.GetTagName().ToString());
		return;
	}

	if (!EditMapDataAsset->GetEditMap().Contains(BuildData->Edit))
	{
		UE_LOG(LogFBC, Error, TEXT("BuildAbility: Received invalid edit %d for structure type %s"),
			BuildData->Edit,
			*BuildData->StructureTag.GetTagName().ToString());
		return;
	}

	TSubclassOf<APlacedStructure> StructureActorClass = EditMapDataAsset->GetEditMap()[BuildData->Edit].StructureClass;
	if (!IsValid(StructureActorClass))
	{
		UE_LOG(LogFBC, Error,
			TEXT("BuildAbility: No valid structure classes found for structure tag %s. Could not spawn structure."),
			*BuildData->StructureTag.GetTagName().ToString());
		return;
	}

	// Finally, make sure we have enough materials
	CachedMaterialType = BuildData->MaterialType;
	if (!CommitAbility(GetCurrentAbilitySpecHandle(), CurrentActorInfo, GetCurrentActivationInfo()))
	{
		UE_LOG(LogFBC, Warning, TEXT("BuildAbility: Insufficient materials"));
		return;
	}
	
	// Build request validated
	
	// Spawn and initialize structure
	APlacedStructure* PlacedStructure = GetWorld()->SpawnActorDeferred<APlacedStructure>(StructureActorClass, BuildingTransform);

	// Things that need to happen before actor is spawned
	
	UGameplayStatics::FinishSpawningActor(PlacedStructure, BuildingTransform);
	
	PlacedStructure->SetResourceType(CachedMaterialType);
	PlacedStructure->SetEditBitfield(BuildData->Edit);
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
		TargetingActor->SetEditMap(StructureInfo->GetEditMapAsset(Tag)->GetEditMap());
		TargetingActor->SetPlacementStrategy(StrategyWorldSubsystem->GetStrategy(Tag));
		TargetingActor->SetStructureTag(Tag);
		TargetingActor->SetStructureEdit(GetCurrentStructureEdit(Tag));
	}
}

int32 UBuildAbility::GetCurrentStructureEdit(FGameplayTag StructureTag)
{
	if (CurrentStructureEdits.Contains(StructureTag))
	{
		return CurrentStructureEdits[StructureTag];
	}
	else
	{
		int32 DefaultEdit = StructureInfo->GetDefaultEdit(StructureTag);
		CurrentStructureEdits.Add(StructureTag, DefaultEdit);
		return DefaultEdit;
	}
}

void UBuildAbility::OnStartEdit(const FGameplayTag Tag, int32 Count)
{
	if (Count == 0)
	{
		// Edit ability ended
		AddAbilityInputMappingContext();

		// Save final edit
		CurrentStructureEdits.Add(TargetingActor->GetStructureTag(), TargetingActor->GetStructureEdit());
	}
	else
	{
		// Edit ability started
		RemoveAbilityInputMappingContext();
	}
}