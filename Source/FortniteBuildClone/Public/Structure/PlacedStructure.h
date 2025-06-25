// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Data/BitGrid.h"
#include "Data/StructureInfoDataAsset.h"
#include "GameFramework/Actor.h"
#include "Interface/Traversable.h"
#include "PlacedStructure.generated.h"

class UDestructionSubsystem;
class USplineComponent;

UENUM()
enum ENeighborRemovalGroundUpdateRule
{
	NRG_None,
	NRG_Local,
	NRG_Multicast
};

UCLASS()
class FORTNITEBUILDCLONE_API APlacedStructure : public AActor, public ITraversable
{
	GENERATED_BODY()

public:
	APlacedStructure();
	
	FGameplayTag GetStructureTag() const { return StructureTag; }
	void SetStructureTag(FGameplayTag Tag) { StructureTag = Tag; }
	
	// Destroys and unregisters the structure without checking for groundedness
	// Prompts nearby structures to start destroying if not grounded
	UFUNCTION(BlueprintCallable)
	void FinishStructureDestruction();

	// Sets the visibility of the structure's static mesh. Only applies locally - visibility does not replicate
	UFUNCTION(BlueprintCallable)
	void SetStructureMeshVisibility(bool bIsVisible);

	// Set visibility of Blueprint-only components (e.g. doors)
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "SetStructureMeshVisibility")
	void BP_SetStructureVisibility(bool bIsVisible);

	UFUNCTION(BlueprintCallable)
	int32 GetEditBitfield() const { return EditBitfield; }

	UFUNCTION(BlueprintCallable)
	void SetEditBitfield(int32 InEditBitfield) { EditBitfield = InEditBitfield; }
	
	virtual const TArray<USplineComponent*> GetLedges_Implementation() const override { return Ledges; }
	virtual const TMap<USplineComponent*, USplineComponent*> GetOppositeLedges_Implementation() const override { return OppositeLedges; }

	void SetResourceType(EFBCResourceType InResourceType);
	EFBCResourceType GetResourceType() const { return ResourceType;}

	const TSet<APlacedStructure*>& GetNeighbors() const { return Neighbors; }
	void RemoveNeighbor(APlacedStructure* Structure, ENeighborRemovalGroundUpdateRule GroundUpdateRule = NRG_Multicast);
	void AddNeighbor(APlacedStructure* Structure);

	UFUNCTION(BlueprintCallable)
	void ReportNeighbors();
	
	void DisableStructure();
	
	// Initiates destroying the structure after a delay if structure isn't grounded
	UFUNCTION(NetMulticast, Reliable)
	void NotifyGroundUpdate();
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Ability System")
	FGameplayTag StructureTag{};

	UPROPERTY(EditDefaultsOnly)
	float DestructionDelay{.2f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<EFBCResourceType, UMaterialInstance*> MaterialMap{};
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay() override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Replicated, Category = "Editing")
	FBitGrid EditBitfield{};
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> StaticMesh;

	UPROPERTY(BlueprintReadWrite)
	TArray<USplineComponent*> Ledges;
	
	UPROPERTY(BlueprintReadWrite)
	TMap<USplineComponent*, USplineComponent*> OppositeLedges{};

	UFUNCTION(BlueprintNativeEvent)
	void SetStructureMeshMaterial(UMaterialInstance* Material);
private:
	FTimerHandle GroundCheckTimerHandle;
	
	// Returns true if the structure has a path to a structure connected to the ground
	bool IsGrounded();

	bool bIsGroundedCached{};
	double GroundCacheTimestamp{};

	void SetCacheOnStructures(TSet<APlacedStructure*>& Structures, bool bIsGrounded);
	void SetGroundCache(bool bIsGrounded);
	bool IsGroundCacheValid() const;
	bool GetGroundCache() const { return bIsGroundedCached; }

	// Set MaterialType to something invalid initially to force replication
	// Otherwise, Wood (0) structures being placed will not trigger the OnRep function
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_ResourceType)
	EFBCResourceType ResourceType{EFBCResourceType::FBCMat_Max};
	
	UFUNCTION()
	void OnRep_ResourceType(EFBCResourceType NewResourceType);
	
	void UpdateMeshMaterial();

	TObjectPtr<UDestructionSubsystem> DestructionSubsystem;

	void InitializeNeighbors();
	
	TSet<APlacedStructure*> Neighbors{};
	bool bIsGroundingStructure{};
};