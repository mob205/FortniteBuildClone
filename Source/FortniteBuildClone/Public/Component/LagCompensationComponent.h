// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Containers/Deque.h"
#include "LagCompensationComponent.generated.h"

class AGameStateBase;
class AFBCCharacter;

inline constexpr int MaxHitboxes = 10;

struct FHitboxData
{
	TStaticArray<FTransform, MaxHitboxes> Hitboxes;
	float Time;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FORTNITEBUILDCLONE_API ULagCompensationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULagCompensationComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
protected:
	virtual void BeginPlay() override;

	// How long hitbox data is held for, in seconds
	UPROPERTY(EditDefaultsOnly)
	float HistoryDuration{};
private:
	void UpdateHistory();

	TObjectPtr<AFBCCharacter> FBCOwner{};
	TObjectPtr<AGameStateBase> GameState{};
	
	// This can also be a TArray if the number of hitboxes needs to be variable
	TDeque<FHitboxData> PositionHistory{};
};
