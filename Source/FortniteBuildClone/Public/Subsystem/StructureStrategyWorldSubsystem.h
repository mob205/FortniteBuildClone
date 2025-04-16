// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/WorldSubsystem.h"
#include "StructureStrategyWorldSubsystem.generated.h"

class UPlacementStrategy;
class UStructureInfoDataAsset;

/**
 * 
 */
UCLASS()
class FORTNITEBUILDCLONE_API UStructureStrategyWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UPlacementStrategy* GetStrategy(FGameplayTag StructureTag);

	virtual void OnWorldBeginPlay(UWorld& InWorld) override;

	UStructureStrategyWorldSubsystem();
	
protected:
	UStructureInfoDataAsset* StructureInfo;

	UPROPERTY()
	TMap<FGameplayTag, UPlacementStrategy*> Strategies{};
};

