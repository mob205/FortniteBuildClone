// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "StructureInfoDataAsset.generated.h"

class UInputAction;
class APlacedStructure;
class AGhostPreviewStructure;
class UPlacementStrategy;

USTRUCT(BlueprintType)
struct FStructureClasses
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AGhostPreviewStructure> TargetingActorClass{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<APlacedStructure> StructureActorClass{};
};

USTRUCT(BlueprintType)
struct FStructureInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UInputAction> InputAction{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag GameplayTag{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FStructureClasses StructureClasses{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UPlacementStrategy> PlacementStrategyClass{};
};


UCLASS(BlueprintType, Blueprintable)
class FORTNITEBUILDCLONE_API UStructureInfoDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StructureInfo")
	FGameplayTag GetTagFromInput(const UInputAction* InputAction) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Structure Info")
	bool GetStructureClasses(const FGameplayTag& StructureTag, FStructureClasses& Classes);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Structure Info")
	TSubclassOf<AGhostPreviewStructure> GetGhostClass(const FGameplayTag& StructureTag);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Structure Info")
	TSubclassOf<APlacedStructure> GetStructureActorClass(const FGameplayTag& StructureTag);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Structure Info")
	TSubclassOf<UPlacementStrategy> GetPlacementStrategyClass(const FGameplayTag& StructureTag);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Structure Info")
	TMap<FGameplayTag, TSubclassOf<UPlacementStrategy>> GetAllPlacementStrategyClasses();
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<FStructureInfo> StructureInfo{};

	bool bHasInitializedMaps{};
	
	TMap<FGameplayTag, FStructureClasses> StructureClasses{};
	TMap<FGameplayTag, TSubclassOf<UPlacementStrategy>> StrategyClasses{};
	void InitializeMaps();
};
