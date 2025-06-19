// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FBCGameState.generated.h"

class APlacedStructure;

/**
 * 
 */
UCLASS()
class FORTNITEBUILDCLONE_API AFBCGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	UFUNCTION(NetMulticast, Reliable)
	void DisableStructures(const TArray<APlacedStructure*>& Structures);
};
