// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Build/StructureTargetingActor.h"
#include "FBCBlueprintLibrary.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystem/Abilities/Build/BuildTargetData.h"
#include "Interface/MaterialSwitchable.h"
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
	
	if (!IsValid(CurrentStrategy)) { return; }
	
	FTransform ResultTransform{};

	APawn* Player = Cast<APawn>(Avatar);

	bool bReceivedValidLocation = CurrentStrategy->GetTargetingLocation(Player, CurrentRotationOffset, ResultTransform);
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
	if (!bHasValidTarget)
	{
		return;
	}

	// Fallback to Wood if can't get the selected material
	EFBCMaterialType MaterialType = EFBCMaterialType::FBCMat_Wood;
	
	if (Avatar->Implements<UMaterialSwitchable>())
	{
		MaterialType = IMaterialSwitchable::Execute_GetCurrentMaterial(Avatar);
	}
	
	FGameplayAbilityTargetDataHandle Handle{
		new FBuildTargetData{
			CurrentStructureTag,
			MaterialType,
			GetActorLocation(),
			GetActorRotation()
		}
	};
	
	TargetDataReadyDelegate.Broadcast(Handle);
}

void AStructureTargetingActor::SetGhostMesh(UStaticMesh* InGhostMesh)
{
	GhostMeshComponent->SetStaticMesh(InGhostMesh);
}

void AStructureTargetingActor::SetPlacementStrategy(UPlacementStrategy* InStrategy)
{
	CurrentStrategy = InStrategy;
}

void AStructureTargetingActor::ValidateGhost() const
{
	GhostMeshComponent->SetVisibility(true);
	GhostMeshComponent->SetMaterial(0, ValidGhostMaterial);
}

void AStructureTargetingActor::InvalidateGhost() const
{
	GhostMeshComponent->SetVisibility(true);
	GhostMeshComponent->SetMaterial(0, InvalidGhostMaterial);

}

void AStructureTargetingActor::HideGhost() const
{
	GhostMeshComponent->SetVisibility(false);
}



