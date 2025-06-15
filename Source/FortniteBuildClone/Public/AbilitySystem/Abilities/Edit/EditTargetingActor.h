// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Data/EditMapDataAsset.h"
#include "EditTargetingActor.generated.h"

class AEditSelectionTile;
class UMaterialInstance;

UCLASS()
class FORTNITEBUILDCLONE_API AEditTargetingActor : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	AEditTargetingActor();

	UFUNCTION(BlueprintCallable)
	void RegisterSelectionTile(AEditSelectionTile* SelectionTile, int BitIndex);
	
	virtual void SetSelectedEdit(int32 InBitfield);

	void InitializeEditTargeting(APlayerController* PC, float Range, const FEditMap* InEditMap);

	virtual void SetSelection(bool bNewSelecting) { bIsSelecting = bNewSelecting; }

	virtual void ConfirmTargetingAndContinue() override;

	UFUNCTION(BlueprintCallable)
	void ResetEdit();

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInstance> GhostMaterial{};

	UPROPERTY(EditDefaultsOnly)
	FBitGrid ResetState{};
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> GhostMeshComponent;

	TObjectPtr<APlayerController> AvatarPC;

	const FEditMap* EditMap;

	TSet<AEditSelectionTile*> SelectionTiles{};

	virtual void StartSelecting() {};
	virtual void ProcessSelecting();
	virtual void EndSelecting();

	virtual void StartTargeting(UGameplayAbility* Ability) override;

	virtual void Tick(float DeltaSeconds) override;

protected:
	bool bFirstTileInitialStatus{};
	TSet<AEditSelectionTile*> EncounteredTiles{};
	int32 CurrentEdit{};

	virtual void UpdateSelectionTiles();
	AEditSelectionTile* GetCurrentTile() const;

private:
	float TargetingRange{};
	
	bool bIsSelecting{};
	bool bLastSelecting{};
	
};
