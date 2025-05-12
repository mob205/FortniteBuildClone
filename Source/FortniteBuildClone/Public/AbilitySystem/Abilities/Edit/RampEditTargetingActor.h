// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Edit/EditTargetingActor.h"
#include "RampEditTargetingActor.generated.h"

/**
 * 
 */
UCLASS()
class FORTNITEBUILDCLONE_API ARampEditTargetingActor : public AEditTargetingActor
{
	GENERATED_BODY()

public:
	// Returns true if edits allow the structure to change rotation
	virtual FORCEINLINE bool IsEditRotatingAllowed() const override { return true; }
	
protected:
	virtual void StartSelecting() override;
	virtual void ProcessSelecting() override;
	virtual void EndSelecting() override;

	virtual void SetSelectedEdit(int32 InBitfield) override;

	virtual void UpdateSelectionTiles() override;
private:
	void InternalUpdateEdit(int32 InBitfield);

	void RotateToCenter(int CenterIndex);
	
	bool bIsFirstTileSet{};
	int FirstTileIndex{};
	int LastSelectedCornerIndex{};
	
	FBitGrid SelectedTiles{};
};
