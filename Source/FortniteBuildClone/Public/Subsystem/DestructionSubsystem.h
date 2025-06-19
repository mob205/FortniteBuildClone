// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structure/PlacedStructure.h"
#include "Subsystems/WorldSubsystem.h"
#include "DestructionSubsystem.generated.h"

class AFBCGameState;
class APlacedStructure;

inline constexpr int DestructionCellLength{8};

UCLASS()
class FORTNITEBUILDCLONE_API UDestructionSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	/*
	* Queues destruction and destroys only a few actors per tick to avoid hitches from deleting too many actors at once
	* An object pool can also be used, but due to the vast amounts of different structures from edits and the relative infrequency
	* of structures being placed, this doesn't seem like an easier option
	*/
	void QueueDestruction(APlacedStructure* Structure);

	FORCEINLINE static FIntVector GetDestructionCoordinate(const FVector& WorldLocation);

	virtual TStatId GetStatId() const override { return TStatId(); }
protected:
	virtual void OnWorldBeginPlay(UWorld& InWorld) override;
	virtual void Tick(float DeltaTime) override;

private:
	TObjectPtr<AFBCGameState> GameState{};
	TQueue<APlacedStructure*> DestructionQueue;
	TArray<APlacedStructure*> DisableStructureBuffer;
};
