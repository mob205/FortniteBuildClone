// Fill out your copyright notice in the Description page of Project Settings.

#include "Build/StructureTargetingActor.h"
#include "FBCBlueprintLibrary.h"
#include "Abilities/GameplayAbility.h"
#include "GridWorldSubsystem.h"
#include "Build/PlacementStrategy/PlacementStrategy.h"


AStructureTargetingActor::AStructureTargetingActor()
{
	PrimaryActorTick.bCanEverTick = true;

	GhostActorComponent = CreateDefaultSubobject<UChildActorComponent>("Ghost Actor");
	RootComponent = GhostActorComponent;
	
	bDestroyOnConfirmation = false;
}

void AStructureTargetingActor::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	SetActorScale3D({1, 1, 1});
	GridSubsystem = GetWorld()->GetSubsystem<UGridWorldSubsystem>();
}

void AStructureTargetingActor::Tick(float DeltaTime)
{
	if (!IsValid(CurrentStrategy)) { return; }
	
	FTransform ResultTransform{};
	
	if (CurrentStrategy->GetTargetingLocation(PrimaryPC, GridSubsystem,
		TargetingRange, CurrentRotationOffset, ResultTransform))
	{
		SetActorTransform(ResultTransform);
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
	FGameplayAbilityTargetData_LocationInfo LocationInfo{};

	LocationInfo.SourceLocation.LocationType = EGameplayAbilityTargetingLocationType::ActorTransform;
	LocationInfo.SourceLocation.SourceActor = GetOwner();

	LocationInfo.TargetLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
	LocationInfo.TargetLocation.LiteralTransform = GetActorTransform();

	const FGameplayAbilityTargetDataHandle DataHandle{new FGameplayAbilityTargetData_LocationInfo{LocationInfo}};

	TargetDataReadyDelegate.Broadcast(DataHandle);
}

void AStructureTargetingActor::SetGhostActorClass(const TSubclassOf<AActor>& InGhostActorClass)
{
	GhostActorComponent->SetChildActorClass(InGhostActorClass);
}

void AStructureTargetingActor::SetPlacementStrategy(const TSubclassOf<UPlacementStrategy>& StrategyClass)
{
	// If needed, can cache strategies instead of making new ones each time for some optimization when building a lot
	CurrentStrategy = NewObject<UPlacementStrategy>(this, StrategyClass);
}


