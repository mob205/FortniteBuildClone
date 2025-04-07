// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "PlacedStructure.generated.h"

class UGridWorldSubsystem;

UCLASS()
class FORTNITEBUILDCLONE_API APlacedStructure : public AActor
{
	GENERATED_BODY()

public:
	APlacedStructure();
	
	FGameplayTag GetStructureTag() const { return StructureTag; }
	void SetStructureTag(FGameplayTag Tag) { StructureTag = Tag; }

	UFUNCTION(BlueprintCallable)
	void StartStructureDestruction();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Ability System")
	FGameplayTag StructureTag{};

	UPROPERTY(EditDefaultsOnly)
	float DestructionDelay{.2f};
	
	TObjectPtr<UGridWorldSubsystem> GridWorldSubsystem;
	
	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	void FinishStructureDestruction();
};
