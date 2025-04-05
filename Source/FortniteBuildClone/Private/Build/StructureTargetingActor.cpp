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

	bool bIsValidTargetLocation = CurrentStrategy->GetTargetingLocation(CurrentRotationOffset, ResultTransform)
		&& !GridSubsystem->IsOccupied(ResultTransform, CurrentStructureTag);
	
	if (bIsValidTargetLocation)
	{
		SetActorTransform(ResultTransform);

		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Green, FString::Printf(TEXT("Can place: true")));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Red, FString::Printf(TEXT("Can place: false")));
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
	// Check if there is a cached strategy
	if (CachedStrategies.Contains(StrategyClass))
	{
		CurrentStrategy = CachedStrategies[StrategyClass];
		return;
	}

	// Create new strategy
	CurrentStrategy = NewObject<UPlacementStrategy>(this, StrategyClass);
	CurrentStrategy->InitializeStrategy(Avatar, GridSubsystem);

	CachedStrategies.Add(StrategyClass, CurrentStrategy);
}


