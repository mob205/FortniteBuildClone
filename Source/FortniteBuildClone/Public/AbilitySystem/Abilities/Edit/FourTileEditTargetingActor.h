// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EditTargetingActor.h"
#include "FourTileEditTargetingActor.generated.h"

UCLASS()
class FORTNITEBUILDCLONE_API AFourTileEditTargetingActor : public AEditTargetingActor
{
	GENERATED_BODY()
	
public:
	virtual void SetSelectedEdit(int32 InBitfield) override;
};
