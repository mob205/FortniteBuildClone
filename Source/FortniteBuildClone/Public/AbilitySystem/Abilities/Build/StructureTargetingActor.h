// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Data/EditMapDataAsset.h"
#include "StructureTargetingActor.generated.h"

class UBuildResourceComponent;
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
	
	// Adds a rotation offset. Each call adds a 90-degree turn around Z-axis.
	UFUNCTION(BlueprintCallable)
	void AddRotationOffset() { CurrentRotationOffset = (CurrentRotationOffset + 1) % 4; };

	void SetAvatar(APawn* InAvatar) { Avatar = InAvatar; }
	void SetResourceComponent(UBuildResourceComponent* InResourceComponent) { ResourceComponent = InResourceComponent; }
	void SetEditMap(const FEditMap& InEditMap) { CurrentEditMap = &InEditMap; }
	
	void SetPlacementStrategy(UPlacementStrategy* InStrategy);
	
	void SetStructureTag(FGameplayTag InStructureTag) { CurrentStructureTag = InStructureTag; }
	FGameplayTag GetStructureTag() const { return CurrentStructureTag; }

	void SetStructureEdit(int32 Edit);
	int32 GetStructureEdit() const { return CurrentStructureEdit; }

	void ToggleEditTarget(bool bNewIsEditTarget);
	
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> GhostMeshComponent;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInstance> ValidGhostMaterial{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInstance> InvalidGhostMaterial{};
	
private:
	bool bHasValidTarget{};
	bool bIsEditTarget{};

	int32 CurrentStructureEdit{};
	const FEditMap* CurrentEditMap{};
	
	// Number of 90 degree turns to offset from standard rotation
	int CurrentRotationOffset{};
	
	FIntVector GridCoordinateLocation{};

	TObjectPtr<UPlacementStrategy> CurrentStrategy;
	TObjectPtr<APawn> Avatar;
	TObjectPtr<UBuildResourceComponent> ResourceComponent;
	
	FGameplayTag CurrentStructureTag{};
	

	void ValidateGhost() const;
	void InvalidateGhost() const;
	void HideGhost() const;
};