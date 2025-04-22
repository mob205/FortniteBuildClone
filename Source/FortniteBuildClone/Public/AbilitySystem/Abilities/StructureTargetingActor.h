// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "StructureTargetingActor.generated.h"

class AGhostPreviewStructure;
class APlayerController;
class AGhostStructureActor;
class UPlacementStrategy;
class UMaterialInstance;

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
	void SetGhostMesh(UStaticMesh* InGhostMesh);

	// Adds a rotation offset. Each call adds a 90-degree turn around Z-axis.
	UFUNCTION(BlueprintCallable)
	void AddRotationOffset() { CurrentRotationOffset = (CurrentRotationOffset + 1) % 4; };

	void SetPlacementStrategy(UPlacementStrategy* InStrategy);

	void SetAvatar(APawn* InAvatar) { Avatar = InAvatar; }

	void SetStructureTag(FGameplayTag InStructureTag) { CurrentStructureTag = InStructureTag; }
protected:
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> GhostMeshComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInstance> ValidGhostMaterial{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInstance> InvalidGhostMaterial{};
	
private:
	bool bHasValidTarget{};

	// Number of 90 degree turns to offset from standard rotation
	int CurrentRotationOffset{};
	
	FIntVector GridCoordinateLocation{};

	TObjectPtr<UPlacementStrategy> CurrentStrategy;
	TObjectPtr<APawn> Avatar;
	
	FGameplayTag CurrentStructureTag{};

	void ValidateGhost() const;
	void InvalidateGhost() const;
	void HideGhost() const;
};
