// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditMapDataAsset.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "StructureResourceTypes.h"
#include "StructureInfoDataAsset.generated.h"

class AEditTargetingActor;
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

USTRUCT(BlueprintType)
struct FEditTargetingClassInfo
{

	GENERATED_BODY()
	// Class of targeting actor to use 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AEditTargetingActor> TargetingActorClass{};
	
	// Server will validate edits and ignore rotations if not allowed
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanRotate{};
};

UCLASS(BlueprintType, Blueprintable)
class FORTNITEBUILDCLONE_API UStructureInfoDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "StructureInfo")
	FGameplayTag GetTagFromInput(const UInputAction* InputAction) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Structure Info")
	UStaticMesh* GetMesh(const FGameplayTag& StructureTag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Structure Info")
	TSubclassOf<APlacedStructure> GetStructureActorClass(const FGameplayTag& StructureTag) const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Structure Info")
	TSubclassOf<UPlacementStrategy> GetPlacementStrategyClass(const FGameplayTag& StructureTag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Structure Info")
	TMap<FGameplayTag, TSubclassOf<UPlacementStrategy>> GetAllPlacementStrategyClasses() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Edit Info")
	const UEditMapDataAsset* GetEditMapAsset(const FGameplayTag& StructureTag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Edit Info")
	FEditTargetingClassInfo GetEditTargetingClass(const FGameplayTag& StructureTag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Edit Info")
	int32 GetDefaultEdit(const FGameplayTag& StructureTag) const;
	
protected:
	virtual void PostLoad() override;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FStructureInfo> StructureInfo{};

	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, UEditMapDataAsset*> EditMaps{};

	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, FEditTargetingClassInfo> EditTargetingActors{};
private:

	UPROPERTY(Transient)
	TMap<FGameplayTag, TSubclassOf<APlacedStructure>> StructureClasses{};
	UPROPERTY(Transient)
	TMap<FGameplayTag, UStaticMesh*> Meshes{};
	UPROPERTY(Transient)
	TMap<FGameplayTag, TSubclassOf<UPlacementStrategy>> StrategyClasses{};
	
	void InitializeMaps();
};
