// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "Data/BitGrid.h"
#include "EditTargetingActor.generated.h"

UCLASS()
class FORTNITEBUILDCLONE_API AEditTargetingActor : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	AEditTargetingActor();

	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TMap<FBitGrid, int> EditMap{};

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> GhostMeshComponent;
};
