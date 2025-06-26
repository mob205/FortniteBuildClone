// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Data/BitGrid.h"
#include "Data/StructureInfoDataAsset.h"
#include "GameFramework/Actor.h"
#include "Interface/Traversable.h"
#include "PlacedStructure.generated.h"

class UStructureGroundingComponent;
class UDestructionSubsystem;
class USplineComponent;



UCLASS()
class FORTNITEBUILDCLONE_API APlacedStructure : public AActor, public ITraversable
{
	GENERATED_BODY()

public:
	APlacedStructure();
	
	FGameplayTag GetStructureTag() const { return StructureTag; }
	void SetStructureTag(FGameplayTag Tag) { StructureTag = Tag; }

	// Sets the visibility of the structure's static mesh. Only applies locally - visibility does not replicate
	UFUNCTION(BlueprintCallable)
	void SetStructureMeshVisibility(bool bIsVisible);

	// Set visibility of Blueprint-only components (e.g. doors)
	UFUNCTION(BlueprintImplementableEvent, DisplayName = "SetStructureMeshVisibility")
	void BP_SetStructureVisibility(bool bIsVisible);

	// Structures store their edit bitfields to help initialize edit ability
	UFUNCTION(BlueprintCallable)
	int32 GetEditBitfield() const { return EditBitfield; }
	UFUNCTION(BlueprintCallable)
	void SetEditBitfield(int32 InEditBitfield) { EditBitfield = InEditBitfield; }
	
	virtual const TArray<USplineComponent*> GetLedges_Implementation() const override { return Ledges; }
	virtual const TMap<USplineComponent*, USplineComponent*> GetOppositeLedges_Implementation() const override { return OppositeLedges; }

	void SetResourceType(EFBCResourceType InResourceType);
	EFBCResourceType GetResourceType() const { return ResourceType; }

	UStructureGroundingComponent* GetGroundingComponent() const { return GroundingComponent; }
	
	void DisableStructure();
	

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Ability System")
	FGameplayTag StructureTag{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<EFBCResourceType, UMaterialInstance*> MaterialMap{};
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
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

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UStructureGroundingComponent* GroundingComponent{};
	
	UFUNCTION(BlueprintNativeEvent)
	void SetStructureMeshMaterial(UMaterialInstance* Material);
private:
	// Set MaterialType to something invalid initially to force replication
	// Otherwise, Wood (0) structures being placed will not trigger the OnRep function
	UPROPERTY(Replicated, ReplicatedUsing = OnRep_ResourceType)
	EFBCResourceType ResourceType{EFBCResourceType::FBCMat_Max};
	
	UFUNCTION()
	void OnRep_ResourceType(EFBCResourceType NewResourceType);
	
	void UpdateMeshMaterial();
};