// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "StructureTargetingActor.generated.h"

class APlayerController;
class UMaterialInterface;

UCLASS(Blueprintable, BlueprintType)
class FORTNITEBUILDCLONE_API AStructureTargetingActor : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	AStructureTargetingActor();

	virtual void StartTargeting(UGameplayAbility* Ability) override;
	virtual void Tick(float DeltaTime) override;
	virtual void ConfirmTargetingAndContinue() override;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ExposeOnSpawn = true))
	TObjectPtr<UClass> GhostActorClass;
	
protected:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UChildActorComponent> GhostActorComponent;
	
	TObjectPtr<APlayerController> PC;
};
