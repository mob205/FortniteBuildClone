// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/DestructionSubsystem.h"

constexpr int MaxDeletionsPerTick{5};
constexpr int MaxDisabledActorsPerRPC{10};

void UDestructionSubsystem::QueueDestruction(AActor* Actor)
{
	if (IsValid(Actor))
	{
		DestructionQueue.Enqueue(Actor);
		bQueuedThisFrame = true;
	}
}

void UDestructionSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TRACE_CPUPROFILER_EVENT_SCOPE_STR("UDestructionSubsystem::Tick");
	if (bQueuedThisFrame)
	{
		bQueuedThisFrame = false;
		return;
	}
	
	int NumDeleted{};
	while (!DestructionQueue.IsEmpty() && NumDeleted < MaxDeletionsPerTick)
	{
		AActor* Actor{};
		DestructionQueue.Dequeue(Actor);
		Actor->Destroy();
		++NumDeleted;
	}
}
