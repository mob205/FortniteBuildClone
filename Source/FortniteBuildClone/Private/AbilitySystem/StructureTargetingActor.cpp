// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/StructureTargetingActor.h"

#include "Abilities/GameplayAbility.h"


AStructureTargetingActor::AStructureTargetingActor()
{
	PrimaryActorTick.bCanEverTick = true;

	GhostActorComponent = CreateDefaultSubobject<UChildActorComponent>("Ghost Actor");
	RootComponent = GhostActorComponent;
}

void AStructureTargetingActor::StartTargeting(UGameplayAbility* Ability)
{
	Super::StartTargeting(Ability);

	PC = OwningAbility->GetCurrentActorInfo()->PlayerController.Get();

	UE_LOG(LogTemp, Warning, TEXT("PC equivalence: %d"), PC.Get() == PrimaryPC.Get());
	GhostActorComponent->SetChildActorClass(GhostActorClass);
	GhostActorComponent->CreateChildActor();
	
	SetActorRotation(PC->GetControlRotation());
	SnapRotation();
}

void AStructureTargetingActor::Tick(float DeltaTime)
{
	check(PC);

	FVector ViewStart{};
	FRotator ViewRot{};

	PC->GetPlayerViewPoint(ViewStart, ViewRot);

	const FVector ViewDir = ViewRot.Vector();
	FVector ViewEnd = ViewStart + (ViewDir * 1000); // TODO: Remove magic number

	FHitResult HitResult{};
	if (GetWorld()->LineTraceSingleByChannel(HitResult, ViewStart, ViewEnd, ECollisionChannel::ECC_Visibility))
	{
		SetActorLocation(HitResult.ImpactPoint);
	}
	
	SnapRotation();
}

void AStructureTargetingActor::ConfirmTargetingAndContinue()
{
	FGameplayAbilityTargetData_LocationInfo LocationInfo{};

	LocationInfo.SourceLocation.LocationType = EGameplayAbilityTargetingLocationType::ActorTransform;
	LocationInfo.SourceLocation.SourceActor = GetOwner();

	LocationInfo.TargetLocation.LocationType = EGameplayAbilityTargetingLocationType::LiteralTransform;
	LocationInfo.TargetLocation.LiteralTransform = GetActorTransform();
	
	FGameplayAbilityTargetDataHandle DataHandle{new FGameplayAbilityTargetData_LocationInfo{LocationInfo}};

	TargetDataReadyDelegate.Broadcast(DataHandle);
}

void AStructureTargetingActor::SnapRotation()
{
	FRotator Rot = GetActorRotation();
	Rot.Yaw = FMath::RoundToFloat(Rot.Yaw / 90) * 90;
	Rot.Pitch = 0;
	Rot.Roll = 0;
	SetActorRotation(Rot);
}
