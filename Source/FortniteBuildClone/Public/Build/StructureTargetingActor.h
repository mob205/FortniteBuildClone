// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "StructureTargetingActor.generated.h"

class UGridWorldSubsystem;
class APlayerController;
class AGhostStructureActor;
class UPlacementStrategy;

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
	void SetGhostActorClass(const TSubclassOf<AActor>& InGhostActorClass);

	// Adds a rotation offset. Each call adds a 90-degree turn around Z-axis.
	UFUNCTION(BlueprintCallable)
	void AddRotationOffset() { CurrentRotationOffset = (CurrentRotationOffset + 1) % 4; };

	void SetPlacementStrategy(const TSubclassOf<UPlacementStrategy>& StrategyClass);

	void SetAvatar(APawn* InAvatar) { Avatar = InAvatar; }

	void SetStructureTag(FGameplayTag InStructureTag) { CurrentStructureTag = InStructureTag; }
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UChildActorComponent> GhostActorComponent;
	
private:
	// Number of 90 degree turns to offset from standard rotation
	int CurrentRotationOffset{};
	
	FIntVector GridCoordinateLocation{};

	// Prevent CurrentStrategy from being garbage collected
	UPROPERTY()
	TObjectPtr<UPlacementStrategy> CurrentStrategy;
	
	TObjectPtr<UGridWorldSubsystem> GridSubsystem;

	TObjectPtr<APawn> Avatar;

	FGameplayTag CurrentStructureTag{};

	TMap<TSubclassOf<UPlacementStrategy>, UPlacementStrategy*> CachedStrategies{};
};
