// Fill out your copyright notice in the Description page of Project Settings.


#include "Structure/PlacementStrategy/PlacementStrategy.h"

#include "FBCBlueprintLibrary.h"
#include "Components/BoxComponent.h"
#include "Engine/SCS_Node.h"
#include "Engine/SimpleConstructionScript.h"
#include "FortniteBuildClone/FortniteBuildClone.h"
#include "Structure/PlacedStructure.h"

bool UPlacementStrategy::CanPlace(const FTransform& QueryTransform, int32 Edit)
{
	TArray<AActor*> OverlappingActors{};
	GetNearbyActors(QueryTransform, Edit, OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (APlacedStructure* AsPlacedStructure = Cast<APlacedStructure>(OverlappingActor))
		{
			return true;
		}
		if (UFBCBlueprintLibrary::IsGround(OverlappingActor))
		{
			return true;
		}
	}
	return false;
}

bool UPlacementStrategy::IsOccupied(const FTransform& QueryTransform)
{
	FTransform SnappedTransform = UFBCBlueprintLibrary::SnapTransformToGrid(QueryTransform);
	TArray<AActor*> OverlappingActors{};
	GetNearbyActors(SnappedTransform, OverlappingActors);

	for (AActor* OverlappingActor : OverlappingActors)
	{
		if (APlacedStructure* AsPlacedStructure = Cast<APlacedStructure>(OverlappingActor))
		{
			if (IsStructureOccupying(SnappedTransform, AsPlacedStructure))
			{
				return true;
			}
		}
	}
	return false;
}

bool UPlacementStrategy::IsStructureOccupying(const FTransform& QueryTransform, const APlacedStructure* Structure) const
{
	return IncompatibleStructureTags.HasTagExact(Structure->GetStructureTag()) &&
		UFBCBlueprintLibrary::GetGridCoordinateLocation(QueryTransform.GetLocation()) == UFBCBlueprintLibrary::GetGridCoordinateLocation(Structure->GetActorLocation());
}

void UPlacementStrategy::InitializeStrategy(UStructureInfoDataAsset* InStructureInfo)
{
	StructureInfo = InStructureInfo;
}

FVector UPlacementStrategy::GetViewLocation(const APlayerController* PC, const FCollisionObjectQueryParams& ObjectQueryParams) const
{
	FVector ViewStart{};
	FRotator ViewRot{};

	PC->GetPlayerViewPoint(ViewStart, ViewRot);

	const FVector ViewDir = ViewRot.Vector();
	const FVector ViewEnd = ViewStart + (ViewDir * TargetingRange);

	FHitResult HitResult{};
	if (GetWorld()->LineTraceSingleByObjectType(HitResult, ViewStart, ViewEnd, ObjectQueryParams))
	{
		return HitResult.Location;
	}
	else
	{
		return ViewEnd;
	}
}

void UPlacementStrategy::GetNearbyActors(const FTransform& QueryTransform, TArray<AActor*>& OutActors)
{
	int32 Edit = StructureInfo->GetDefaultEdit(StructureTag);
	GetNearbyActors(QueryTransform, Edit, OutActors);
}

void UPlacementStrategy::GetNearbyActors(const FTransform& QueryTransform, int32 Edit, TArray<AActor*>& OutActors)
{
	AActor* QueryActor = GetQueryActor(Edit);
	
	FTransform SnappedTransform = UFBCBlueprintLibrary::SnapTransformToGrid(QueryTransform);
	QueryActor->SetActorTransform(SnappedTransform);
	
	QueryActor->SetActorEnableCollision(true);
	QueryActor->GetOverlappingActors(OutActors);
	QueryActor->SetActorEnableCollision(false);
}

AActor* UPlacementStrategy::GetQueryActor(int32 Edit)
{
	// We have a cached query actor
	if (QueryActors.Contains(Edit))
	{
		return QueryActors[Edit];
	}


	// No cached actor - we need to make one
	const FEditMap& EditMap = StructureInfo->GetEditMapAsset(StructureTag)->GetEditMap();

	// Placement strategy shouldn't ever get an invalid edit.
	check(EditMap.Contains(Edit));
	
	AActor* OverlapQueryActor = GetWorld()->SpawnActor(OverlapQueryActorClass);

	UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(EditMap[Edit].StructureClass);
	USimpleConstructionScript* SCS = BPClass->SimpleConstructionScript;
	const TArray<USCS_Node*>& Nodes = SCS->GetAllNodes();
	
	for (const auto Node : Nodes)
	{
		UBoxComponent* AsBoxComponent = Cast<UBoxComponent>(Node->ComponentTemplate);
		if (!AsBoxComponent || !AsBoxComponent->GetCollisionEnabled())
		{
			continue;
		}

		UBoxComponent* NewBoxComponent = NewObject<UBoxComponent>(OverlapQueryActor);
		NewBoxComponent->RegisterComponent();
		NewBoxComponent->AttachToComponent(OverlapQueryActor->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		NewBoxComponent->SetRelativeTransform(AsBoxComponent->GetRelativeTransform());
		NewBoxComponent->SetBoxExtent(AsBoxComponent->GetUnscaledBoxExtent(), false);
		NewBoxComponent->SetCollisionObjectType(AsBoxComponent->GetCollisionObjectType());
		NewBoxComponent->SetCollisionResponseToChannels(AsBoxComponent->GetCollisionResponseToChannels());
	}

	OverlapQueryActor->SetActorEnableCollision(false);
	
	QueryActors.Add(Edit, OverlapQueryActor);
	
	return OverlapQueryActor;
}
