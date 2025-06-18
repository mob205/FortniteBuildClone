// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/DestructionSubsystem.h"

#include "FBCBlueprintLibrary.h"
#include "GridSizes.h"
#include "Structure/PlacedStructure.h"

constexpr int MaxDeletionsPerTick{5};

void UDestructionSubsystem::QueueDestruction(APlacedStructure* Structure)
{
	DestructionQueue.Enqueue(Structure);
}

FIntVector UDestructionSubsystem::GetDestructionCoordinate(const FVector& WorldLocation)
{
	return UFBCBlueprintLibrary::GetCustomGridCoordinateLocation(WorldLocation,
		FVector{
			GridSizeHorizontal * DestructionCellLength,
			GridSizeHorizontal * DestructionCellLength,
			GridSizeVertical * DestructionCellLength
		});
}

void UDestructionSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	int NumDeleted{};
	while (!DestructionQueue.IsEmpty() && NumDeleted < MaxDeletionsPerTick)
	{
		APlacedStructure* Structure{};
		DestructionQueue.Dequeue(Structure);
		Structure->Destroy();
		++NumDeleted;
	}
}
