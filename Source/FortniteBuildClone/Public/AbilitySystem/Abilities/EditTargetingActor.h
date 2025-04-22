// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Data/BitGrid.h"
#include "EditTargetingActor.generated.h"

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

	void SetSelectedEdit(int32 InBitfield);

	bool GetSelectedEdit(TSubclassOf<APlacedStructure>& OutStructureClass) const;

	void SetAvatarController(APlayerController* PC) { AvatarPC = PC; }

	void SetSelection(bool bNewSelecting) { bIsSelecting = bNewSelecting; }
protected:
	UPROPERTY(EditDefaultsOnly)
	TMap<FBitGrid, FStructureEditInfo> EditMap{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInstance> GhostMaterial{};
	
	UPROPERTY()
	TObjectPtr<UStaticMeshComponent> GhostMeshComponent;

	TObjectPtr<APlayerController> AvatarPC;

	virtual void BeginPlay() override;

private:
	bool bIsSelecting{}; 
	int32 SelectedEdit{};
};
