// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BitGrid.h"
#include "Engine/DataAsset.h"
#include "EditMapDataAsset.generated.h"

class APlacedStructure;

USTRUCT(BlueprintType)
struct FStructureEditInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UStaticMesh* EditPreviewMesh{};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<APlacedStructure> StructureClass{};
};

using FEditMap = TMap<FBitGrid, FStructureEditInfo>;

/**
 * 
 */
UCLASS()
class FORTNITEBUILDCLONE_API UEditMapDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	const FEditMap& GetEditMap() const { return EditMap;}
protected:
	UPROPERTY(EditDefaultsOnly)
	TMap<FBitGrid, FStructureEditInfo> EditMap;
};
