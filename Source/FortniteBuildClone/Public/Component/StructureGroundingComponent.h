// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Subsystem/DestructionSubsystem.h"
#include "StructureGroundingComponent.generated.h"

UENUM()
enum ENeighborRemovalGroundUpdateRule
{
	NRG_None,
	NRG_Local,
	NRG_Multicast
};

class APlacedStructure;

UCLASS(ClassGroup=(Custom), Within=PlacedStructure, meta=(BlueprintSpawnableComponent))
class FORTNITEBUILDCLONE_API UStructureGroundingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStructureGroundingComponent();

	const TSet<UStructureGroundingComponent*>& GetNeighbors() const { return Neighbors; }

	// Removes a neighbor, which optionally causes a predicted or replicated ground update
	void RemoveNeighbor(UStructureGroundingComponent* Structure, ENeighborRemovalGroundUpdateRule GroundUpdateRule = ENeighborRemovalGroundUpdateRule::NRG_Multicast);
	void AddNeighbor(UStructureGroundingComponent* Structure);

	// Initiates destroying the structure after a delay if structure isn't grounded
	UFUNCTION(NetMulticast, Reliable)
	void NotifyGroundUpdate();

	// Begins local destruction process of this structure on clients
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable)
	void DestroyOnClients();
protected:
	// Called when the game starts
	virtual void BeginPlay() override;


private:
	// Placed structure that owns this component
	TObjectPtr<APlacedStructure> Owner{};
	
	TObjectPtr<UDestructionSubsystem> DestructionSubsystem;

	// Returns true if the structure is connected to ground
	bool IsGrounded();
	
	// Immediately begins the destruction process
	// Notifies a ground update on all neighbors
	void FinishStructureDestruction();

	// If true, this structure is directly in contact with ground
	bool bIsGroundingStructure{};

	// Neighbors are immediately adjacent and in contact with this structure
	TSet<UStructureGroundingComponent*> Neighbors{};

	// Sets up neighbors
	void InitializeNeighbors();
	
	UPROPERTY(EditDefaultsOnly)
	float GroundCheckDelay{.2};
	FTimerHandle GroundCheckTimerHandle;

	// True if IsGrounded() has recently found this structure to be connected to ground
	bool bIsGroundedCached{};
	// Ground cache is only valid on the same frame that it was set
	double GroundCacheTimestamp{};

	// Updates the ground cache on a set of structures
	void SetCacheOnStructures(TSet<UStructureGroundingComponent*>& Structures, bool bIsGrounded);

	void SetGroundCache(bool bIsGrounded);
	bool GetGroundCache() const { return bIsGroundedCached; }

	// Returns true if the ground cache is valid (i.e. it is the same frame it was set on)
	bool IsGroundCacheValid() const;
};
