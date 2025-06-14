// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Data/BitGrid.h"
#include "Data/StructureInfoDataAsset.h"
#include "GameFramework/Actor.h"
#include "Traversal/Traversable.h"
#include "PlacedStructure.generated.h"

class USplineComponent;

UCLASS()
class FORTNITEBUILDCLONE_API APlacedStructure : public AActor, public ITraversable
{
	GENERATED_BODY()

public:
	APlacedStructure();
	
	FGameplayTag GetStructureTag() const { return StructureTag; }
	void SetStructureTag(FGameplayTag Tag) { StructureTag = Tag; }

	// Initiates destroying the structure after a delay
	UFUNCTION(BlueprintCallable)
	void NotifyGroundUpdate();

	// Destroys and unregisters the structure without checking for groundedness
	// Prompts nearby structures to start destroying if not grounded
	UFUNCTION(BlueprintCallable)
	void FinishStructureDestruction();

	// Sets the visibility of the structure's static mesh. Only applies locally - visibility does not replicate
	UFUNCTION(BlueprintCallable)
	void SetStructureMeshVisibility(bool bIsVisible);

	UFUNCTION(BlueprintCallable)
	int32 GetEditBitfield() const { return EditBitfield; }

	UFUNCTION(BlueprintCallable)
	void SetEditBitfield(int32 InEditBitfield) { EditBitfield = InEditBitfield; }
	
	virtual const TArray<USplineComponent*> GetLedges_Implementation() const override { return Ledges; }
	virtual const TMap<USplineComponent*, USplineComponent*> GetOppositeLedges_Implementation() const override { return OppositeLedges; }

	void SetMaterialType(EFBCMaterialType InMaterialType);
	EFBCMaterialType GetMaterialType() const { return MaterialType;}
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Ability System")
	FGameplayTag StructureTag{};

	UPROPERTY(EditDefaultsOnly)
	float DestructionDelay{.2f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<EFBCMaterialType, UMaterialInstance*> MaterialMap{};
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = "Editing")
	FBitGrid EditBitfield{};
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> StaticMesh;

	UPROPERTY(BlueprintReadWrite)
	TArray<USplineComponent*> Ledges;
	
	UPROPERTY(BlueprintReadWrite)
	TMap<USplineComponent*, USplineComponent*> OppositeLedges{};

private:
	FTimerHandle DestroyTimerHandle;
	
	// Returns true if the structure has a path to a structure connected to the ground
	bool IsGrounded();

	bool bIsGroundedCached{};
	double GroundCacheTimestamp{};

	void SetCacheOnStructures(TSet<APlacedStructure*> Structures, bool bIsGrounded);
	void SetGroundCache(bool bIsGrounded);
	bool IsGroundCacheValid() const;
	bool GetGroundCache() const { return bIsGroundedCached; }

	// Set MaterialType to something invalid initially to force replication
	// Otherwise, Wood (0) structures being placed will not trigger OnRep_MaterialType
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_MaterialType)
	EFBCMaterialType MaterialType{EFBCMaterialType::FBCMat_Max};
	
	UFUNCTION()
	void OnRep_MaterialType(EFBCMaterialType NewMaterialType);

	void UpdateMeshMaterial();
};
