// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Build/StructureTargetingActor.h"
#include "FBCBlueprintLibrary.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/Abilities/Build/BuildTargetData.h"
#include "Component/BuildResourceComponent.h"
#include "Structure/PlacementStrategy/PlacementStrategy.h"


AStructureTargetingActor::AStructureTargetingActor()
{
	PrimaryActorTick.bCanEverTick = true;

	GhostMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Ghost Mesh Component");
	GhostMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GhostMeshComponent->SetGenerateOverlapEvents(false);
	SetRootComponent(GhostMeshComponent);
	
	bDestroyOnConfirmation = false;
}

void AStructureTargetingActor::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	SetActorScale3D({1, 1, 1});
}

void AStructureTargetingActor::Tick(float DeltaTime)
{
	bHasValidTarget = false;

	// We don't need to move if we're currently being edited
	if (bIsEditTarget) { return; }

	if (!IsValid(CurrentStrategy)) { return; }

	FTransform ResultTransform{};

	APawn* Player = Cast<APawn>(Avatar);

	bool bReceivedValidLocation = CurrentStrategy->GetTargetingLocation(Player, CurrentRotationOffset, CurrentStructureEdit, ResultTransform);
	bool bIsOccupied{};
	bool bHasEnoughMaterial{ true };
	if (bReceivedValidLocation)
	{
		bIsOccupied = CurrentStrategy->IsOccupied(ResultTransform);
		bHasEnoughMaterial = OwningAbility->CheckCost(OwningAbility->GetCurrentAbilitySpecHandle(), OwningAbility->GetCurrentActorInfo());
	}

	if (bIsOccupied)
	{
		HideGhost();
	}
	else if (bReceivedValidLocation && bHasEnoughMaterial)
	{
		SetActorTransform(ResultTransform);
		bHasValidTarget = true;
		
		ValidateGhost();
	}
	else
	{
		SetActorTransform(ResultTransform);
		InvalidateGhost();
	}
	
	FIntVector NewGridLocation = UFBCBlueprintLibrary::GetGridCoordinateLocation(GetActorLocation());
	if (NewGridLocation != GridCoordinateLocation)
	{
		GridCoordinateLocation = NewGridLocation;
		// On grid coordinate changed - play sound or something
	}
}

void AStructureTargetingActor::ConfirmTargetingAndContinue()
{
	// Editing may have same input as confirming build placement. Ignore placement while being edited
	if (!bHasValidTarget || bIsEditTarget)
	{
		return;
	}

	// Fallback to Wood if can't get the selected material
	EFBCResourceType MaterialType = EFBCResourceType::FBCMat_Wood;
	if (ResourceComponent)
	{
		MaterialType = ResourceComponent->GetCurrentResourceType();
	}
	
	FGameplayAbilityTargetDataHandle Handle{
		new FBuildTargetData{
			CurrentStructureTag,
			MaterialType,
			GetActorLocation(),
			GetActorRotation(),
			CurrentStructureEdit
		}
	};
	
	TargetDataReadyDelegate.Broadcast(Handle);
}

void AStructureTargetingActor::SetPlacementStrategy(UPlacementStrategy* InStrategy)
{
	CurrentStrategy = InStrategy;
}

void AStructureTargetingActor::ValidateGhost() const
{
	GhostMeshComponent->SetVisibility(true);
	GhostMeshComponent->SetMaterial(0, ValidGhostMaterial);
	GhostMeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
}

void AStructureTargetingActor::InvalidateGhost() const
{
	GhostMeshComponent->SetVisibility(true);
	GhostMeshComponent->SetMaterial(0, InvalidGhostMaterial);
	GhostMeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
}

void AStructureTargetingActor::HideGhost() const
{
	GhostMeshComponent->SetVisibility(false);
	GhostMeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
}


void AStructureTargetingActor::SetStructureEdit(int32 Edit)
{
	CurrentStructureEdit = Edit;

	check(CurrentEditMap->Contains(Edit));

	GhostMeshComponent->SetStaticMesh(CurrentEditMap->FindChecked(Edit).EditPreviewMesh);
}

void AStructureTargetingActor::ToggleEditTarget(bool bNewIsEditTarget)
{
	bIsEditTarget = bNewIsEditTarget;

	// Hide mesh if edit target
	GhostMeshComponent->SetVisibility(!bIsEditTarget);

	// Ignore visibility channel if edit target as to not block editing selection
	if (bIsEditTarget)
	{
		GhostMeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	}
	else
	{
		GhostMeshComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	}
}

