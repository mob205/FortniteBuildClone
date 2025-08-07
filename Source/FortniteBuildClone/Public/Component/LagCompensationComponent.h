// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Containers/Deque.h"
#include "LagCompensationComponent.generated.h"

class UBoxComponent;
class AGameStateBase;
class AFBCCharacterBase;
class FLagCompensatedWindow;

DECLARE_LOG_CATEGORY_EXTERN(LogLagCompensation, Log, All);

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

	// Rewinds the owner to its state at Timestamp
	bool TryRewind(float Timestamp);

	// Restores the owner's to the present state
	void RestoreToPresent();

	// Fills OutData with current time and hitbox transforms
	void FillHitboxData(const TArray<UBoxComponent*>& Hitboxes, FHitboxData& OutData);
	
	// Moves player hitboxes to match the stored hitbox data
	void ApplyHitboxData(const TArray<UBoxComponent*>& Hitboxes, const FHitboxData& HitboxData);

	// Searches position history for closest to the timestamp. Returns nullptr if timestamp is invalid
	const FHitboxData* SearchHistory(float Timestamp) const;
	
	TObjectPtr<AFBCCharacterBase> FBCOwner{};
	TObjectPtr<AGameStateBase> GameState{};

	
	// This can also be a TArray if the number of hitboxes needs to be variable
	TDeque<FHitboxData> PositionHistory{};

	bool bIsRewinding{};
	FHitboxData PreRewindHitboxData{};

	friend FLagCompensatedWindow;
};

class FLagCompensatedWindow
{
public:
	// Starts a lag compensation window by rewinding all Targets
	explicit FLagCompensatedWindow(const TArray<AFBCCharacterBase*>& Targets, double Timestamp);
	~FLagCompensatedWindow();

	FLagCompensatedWindow(const FLagCompensatedWindow&) = delete;
	FLagCompensatedWindow(FLagCompensatedWindow&&) = delete;

private:
	TArray<ULagCompensationComponent*> RewindedTargets;
};
