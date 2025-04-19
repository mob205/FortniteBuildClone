// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/StructureStrategyWorldSubsystem.h"

#include "Structure/PlacementStrategy/PlacementStrategy.h"
#include "Data/StructureInfoDataAsset.h"
#include "FortniteBuildClone/FortniteBuildClone.h"
#include "Subsystem/GridWorldSubsystem.h"

UStructureStrategyWorldSubsystem::UStructureStrategyWorldSubsystem()
{
	static ConstructorHelpers::FObjectFinder<UStructureInfoDataAsset> DataAsset(TEXT("/Script/FortniteBuildClone.StructureInfoDataAsset'/Game/Blueprints/Data/DA_StructureInfo.DA_StructureInfo'"));
	if (DataAsset.Succeeded())
	{
		StructureInfo = DataAsset.Object;
	}
	else
	{
		UE_LOG(LogFBC, Error, TEXT("UStructureStrategyWorldSubsystem: Could not find structure info data asset."));
	}
}


UPlacementStrategy* UStructureStrategyWorldSubsystem::GetStrategy(FGameplayTag StructureTag)
{
	if (Strategies.Contains(StructureTag))
	{
		return Strategies[StructureTag];
	}
	UE_LOG(LogFBC, Error, TEXT("UStructureStrategyWorldSubsystem: Attempted to get strategy for invalid structure tag!"));
	return nullptr;
}

void UStructureStrategyWorldSubsystem::OnWorldBeginPlay(UWorld& InWorld)
{
	Super::OnWorldBeginPlay(InWorld);

	UGridWorldSubsystem* GridWorldSubsystem = GetWorld()->GetSubsystem<UGridWorldSubsystem>();
	for (const auto& StrategyClass : StructureInfo->GetAllPlacementStrategyClasses())
	{
		UPlacementStrategy* Strategy = NewObject<UPlacementStrategy>(this, StrategyClass.Value);
		Strategy->InitializeStrategy(GridWorldSubsystem);
		Strategies.Add(StrategyClass.Key, Strategy);
	}
}
