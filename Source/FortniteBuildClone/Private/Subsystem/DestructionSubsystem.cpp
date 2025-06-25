// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/DestructionSubsystem.h"

#include "Structure/PlacedStructure.h"

constexpr int MaxDeletionsPerTick{5};
constexpr int MaxDisabledActorsPerRPC{10};

void UDestructionSubsystem::QueueDestruction(APlacedStructure* Structure)
{
	if (IsValid(Structure))
	{
		DestructionQueue.Enqueue(Structure);
		bQueuedThisFrame = true;
	}
}

void UDestructionSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bQueuedThisFrame)
	{
		bQueuedThisFrame = false;
		return;
	}
	
	int NumDeleted{};
	while (!DestructionQueue.IsEmpty() && NumDeleted < MaxDeletionsPerTick)
	{
		APlacedStructure* Structure{};
		DestructionQueue.Dequeue(Structure);
		Structure->Destroy();
		++NumDeleted;
	}
}
