// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditMapDataAsset.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "StructureInfoDataAsset.generated.h"

class UInputAction;
class APlacedStructure;
class UPlacementStrategy;

USTRUCT(BlueprintType)
struct FStructureInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UInputAction> InputAction{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag GameplayTag{};

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMesh> Mesh{};
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<APlacedStructure> StructureClass{};

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
	UStaticMesh* GetMesh(const FGameplayTag& StructureTag);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Structure Info")
	TSubclassOf<APlacedStructure> GetStructureActorClass(const FGameplayTag& StructureTag);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Structure Info")
	TSubclassOf<UPlacementStrategy> GetPlacementStrategyClass(const FGameplayTag& StructureTag);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Structure Info")
	TMap<FGameplayTag, TSubclassOf<UPlacementStrategy>> GetAllPlacementStrategyClasses();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Edit Info")
	const UEditMapDataAsset* GetEditMap(const FGameplayTag& StructureTag);
protected:
	virtual void PostLoad() override;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FStructureInfo> StructureInfo{};

	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, UEditMapDataAsset*> EditMaps{};
private:

	UPROPERTY(Transient)
	TMap<FGameplayTag, TSubclassOf<APlacedStructure>> StructureClasses{};
	UPROPERTY(Transient)
	TMap<FGameplayTag, UStaticMesh*> Meshes{};
	UPROPERTY(Transient)
	TMap<FGameplayTag, TSubclassOf<UPlacementStrategy>> StrategyClasses{};
	
	void InitializeMaps();
};
