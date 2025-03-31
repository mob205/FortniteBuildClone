// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "StructureTargetingActor.generated.h"

class APlayerController;
class AGhostStructureActor;

UCLASS(Blueprintable, BlueprintType)
class FORTNITEBUILDCLONE_API AStructureTargetingActor : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	AStructureTargetingActor();

	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void Tick(float DeltaTime) override;
	virtual void ConfirmTargetingAndContinue() override;

	// Sets the type of structure for the targeting actor to use. Used for building preview and location selection 
	UFUNCTION(BlueprintCallable)
	void SetGhostActorClass(TSubclassOf<AActor> InGhostActorClass);

	// Rotates the targeting actor to face the player. Also applies rotation offset
	UFUNCTION(BlueprintCallable)
	void RotateToFacePlayer();

	// Adds a rotation offset. Each call adds a 90-degree turn around Z-axis.
	UFUNCTION(BlueprintCallable)
	void AddRotationOffset() { CurrentRotationOffset = (CurrentRotationOffset + 1) % 4; };

	// Sets the building range
	UFUNCTION(BlueprintCallable)
	void SetRange(float InRange) { TargetingRange = InRange; }
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UChildActorComponent> GhostActorComponent;
	
	TObjectPtr<APlayerController> PC;

private:
	// Number of 90 degree turns to offset from standard rotation
	int CurrentRotationOffset{};
	
	FIntVector GridCoordinateLocation{};

	float TargetingRange{};
};
