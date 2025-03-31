// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Abilities/Build/StructureTargetingActor.h"
#include "FBCBlueprintLibrary.h"
#include "Abilities/GameplayAbility.h"


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
}

void AStructureTargetingActor::Tick(float DeltaTime)
{
	FVector ViewStart{};
	FRotator ViewRot{};

	PrimaryPC->GetPlayerViewPoint(ViewStart, ViewRot);

	const FVector ViewDir = ViewRot.Vector();
	const FVector ViewEnd = ViewStart + (ViewDir * TargetingRange);

	FVector TargetLocation{};
	
	FHitResult HitResult{};
	if (GetWorld()->LineTraceSingleByChannel(HitResult, ViewStart, ViewEnd, ECollisionChannel::ECC_Visibility))
	{
		TargetLocation = HitResult.Location;
	}
	else
	{
		TargetLocation = ViewEnd;
	}
	
	SetActorLocation(UFBCBlueprintLibrary::SnapLocationToGrid(TargetLocation));

	RotateToFacePlayer();

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

void AStructureTargetingActor::SetGhostActorClass(TSubclassOf<AActor> InGhostActorClass)
{
	GhostActorComponent->SetChildActorClass(InGhostActorClass);
}

void AStructureTargetingActor::RotateToFacePlayer()
{
	const float Yaw = OwningAbility->GetAvatarActorFromActorInfo()->GetActorRotation().Yaw;
	SetActorRotation({0, UFBCBlueprintLibrary::SnapAngleToGrid(Yaw + 180 + (CurrentRotationOffset * 90)), 0 });
}

