// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "PlacementStrategy.generated.h"

class APlayerController;
class APlacedStructure;
class UStructureInfoDataAsset;

UCLASS(Blueprintable)
class FORTNITEBUILDCLONE_API UPlacementStrategy : public UObject
{
	GENERATED_BODY()

public:
	/**
	 *	Attempts to find a valid location to place a structure of a certain type.
	 *	@param RotationOffset The number of rotations requested by the building player
	 *	@param Edit
	 *	@param OutResult Out parameter to store the valid transform, if found
	 *	@returns true if a valid location was found
	 */
	virtual bool GetTargetingLocation(
		APawn* Player, int RotationOffset, int32 Edit, FTransform& OutResult) { return false; }

	/**
	 * Checks if the strategy's structure would be supported if placed at the specified transform.
	 * Does NOT check for occupation. Use IsOccupied for that
	 * @param QueryTransform The transform the structure will be checked at
	 * @return true if the structure can be successfully placed
	 */
	bool CanPlace(const FTransform& QueryTransform, int32 Edit);

	/**
	 * Checks if the strategy's structure will be blocked by an incompatible structure in the same location
	 * @param QueryTransform The transform the structure will be checked at
	 * @return 
	 */
	bool IsOccupied(const FTransform& QueryTransform);
	
	void InitializeStrategy(UStructureInfoDataAsset* InStructureInfo);

	UPROPERTY(EditDefaultsOnly)
	float TargetingRange{};

protected:
	FVector GetViewLocation(const APlayerController* PC, const FCollisionObjectQueryParams& ObjectQueryParams) const;

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag StructureTag{};

	// Tags for types of structures that this structure cannot be placed on top of (i.e. same grid location)
	UPROPERTY(EditDefaultsOnly)
	FGameplayTagContainer IncompatibleStructureTags{};

	// Returns true if the specified Structure is occupying the QueryTransform such that this strategy's structure can't also be placed there
	virtual bool IsStructureOccupying(const FTransform& QueryTransform, const APlacedStructure* Structure) const;
private:
	// Actor to use to check overlaps
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> OverlapQueryActorClass;

	void GetNearbyActors(const FTransform& QueryTransform, TArray<AActor*>& OutActors);

	void GetNearbyActors(const FTransform& QueryTransform, int32 Edit, TArray<AActor*>& OutActors);

	TObjectPtr<UStructureInfoDataAsset> StructureInfo{};

	AActor* GetQueryActor(int32 Edit);
	
	UPROPERTY()
	TMap<int32, AActor*> QueryActors{};
};
