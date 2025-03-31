// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "StructureInfoDataAsset.generated.h"

class UInputAction;
class APlacedStructure;

USTRUCT(BlueprintType)
struct FStructureClasses
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> TargetingActorClass{};

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
};


UCLASS(BlueprintType, Blueprintable)
class FORTNITEBUILDCLONE_API UStructureInfoDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StructureInfo")
	FGameplayTag GetTagFromInput(const UInputAction* InputAction) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Structure Info")
	bool GetStructureClasses(const FGameplayTag& GameplayTag, FStructureClasses& Classes);

protected:
	UPROPERTY(EditDefaultsOnly)
	TArray<FStructureInfo> StructureInfo{};

	TMap<FGameplayTag, FStructureClasses> StructureClasses{};
	bool bHasInitializedStructureClasses{};

	void InitializeStructureClasses();
	
};
