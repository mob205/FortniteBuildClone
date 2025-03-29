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

	// Default ghost actor class to spawn targeting ghost as
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	TObjectPtr<UClass> DefaultGhostActorClass;

	UFUNCTION(BlueprintCallable)
	void SetGhostActorClass(TSubclassOf<AActor> InGhostActorClass);
protected:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UChildActorComponent> GhostActorComponent;
	
	TObjectPtr<APlayerController> PC;
	
};
