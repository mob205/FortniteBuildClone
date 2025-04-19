// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "PlacementStrategy.generated.h"

class APlayerController;
class UGridWorldSubsystem;

UCLASS(Blueprintable)
class FORTNITEBUILDCLONE_API UPlacementStrategy : public UObject
{
	GENERATED_BODY()

public:
	/**
	 *	Attempts to find a valid location to place a structure of a certain type.
	 *	@param RotationOffset The number of rotations requested by the building player
	 *	@param OutResult Out parameter to store the valid transform, if found
	 *	@returns true if a valid location was found
	 */
	virtual bool GetTargetingLocation(
		APawn* Player, int RotationOffset, FTransform& OutResult) { return false; }

	/**
	 * @param QueryTransform The transform to check structure's placement of
	 * @return true if the structure can be succesfully placed
	 */
	bool CanPlace(const FTransform& QueryTransform) const;

	void InitializeStrategy(UGridWorldSubsystem* GridSubsystem);
	
	UPROPERTY(EditDefaultsOnly)
	float TargetingRange{};

protected:
	TObjectPtr<AActor> OverlapQueryActor{};
	TObjectPtr<UGridWorldSubsystem> GridSubsystem{};

	FVector GetViewLocation(APlayerController* PC, const FCollisionObjectQueryParams& ObjectQueryParams) const;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag StructureTag{};
private:
	// Actor to use to check overlaps
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> OverlapQueryActorClass;
};
