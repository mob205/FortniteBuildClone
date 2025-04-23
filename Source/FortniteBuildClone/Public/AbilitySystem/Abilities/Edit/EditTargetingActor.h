// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/BitGrid.h"
#include "EditTargetingActor.generated.h"

class AEditSelectionTile;
class APlacedStructure;
class UMaterialInstance;

USTRUCT(BlueprintType)
struct FStructureEditInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh* EditPreviewMesh{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<APlacedStructure> StructureClass{};
};

UCLASS()
class FORTNITEBUILDCLONE_API AEditTargetingActor : public AActor
{
	GENERATED_BODY()

public:
	AEditTargetingActor();

	UFUNCTION(BlueprintCallable)
	void RegisterSelectionTile(AEditSelectionTile* SelectionTile, int BitIndex);
	
	void SetSelectedEdit(int32 InBitfield);

	bool GetSelectedEdit(TSubclassOf<APlacedStructure>& OutStructureClass) const;

	void InitializeEditTargeting(APlayerController* PC, float Range);

	void SetSelection(bool bNewSelecting) { bIsSelecting = bNewSelecting; }
protected:
	UPROPERTY(EditDefaultsOnly)
	TMap<FBitGrid, FStructureEditInfo> EditMap{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInstance> GhostMaterial{};
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> GhostMeshComponent;

	TObjectPtr<APlayerController> AvatarPC;

	TSet<AEditSelectionTile*> SelectionTiles{};

	virtual void ProcessSelecting();
	virtual void EndSelecting();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
private:
	float TargetingRange{};
	
	bool bIsSelecting{};
	bool bLastSelecting{};
	bool bFirstTileInitialStatus{};
	TSet<AEditSelectionTile*> EncounteredTiles{};
	
	int32 CurrentEdit{};

	void UpdateSelectionTiles();

	AEditSelectionTile* GetCurrentTile() const;
};
