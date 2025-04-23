// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Build/StructureTargetingActor.h"
#include "FBCBlueprintLibrary.h"
#include "Abilities/GameplayAbility.h"
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
	
	FGameplayAbilityTargetData_LocationInfo LocationInfo{};

	LocationInfo.SourceLocation.LocationType = EGameplayAbilityTargetingLocationType::ActorTransform;
	LocationInfo.SourceLocation.SourceActor = GetOwner();

	LocationInfo.TargetLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
	LocationInfo.TargetLocation.LiteralTransform = GetActorTransform();

	const FGameplayAbilityTargetDataHandle DataHandle{new FGameplayAbilityTargetData_LocationInfo{LocationInfo}};

	TargetDataReadyDelegate.Broadcast(DataHandle);
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



