// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/LagCompensationComponent.h"

#include "Components/BoxComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FBCCharacter.h"

DEFINE_LOG_CATEGORY(LogLagCompensation)


ULagCompensationComponent::ULagCompensationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULagCompensationComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#if WITH_EDITOR
	if (GetOwner()->HasAuthority())
	{
		UpdateHistory();
	}
#else if UE_SERVER
	UpdateHistory();
#endif
	
}

void ULagCompensationComponent::BeginPlay()
{
	Super::BeginPlay();

	FBCOwner = Cast<AFBCCharacter>(GetOwner());
	GameState = UGameplayStatics::GetGameState(this);
}

void ULagCompensationComponent::UpdateHistory()
{
	// Push new data
	PositionHistory.PushFirst({});
	FillHitboxData(FBCOwner->GetHitboxes(), PositionHistory.First());

	const float CurrentTime = GameState->GetServerWorldTimeSeconds();

	// Clear expired data
	while (PositionHistory.Last().Time < CurrentTime - HistoryDuration)
	{
		PositionHistory.PopLast();
	}
}

bool ULagCompensationComponent::TryRewind(float Timestamp)
{
	const FHitboxData* HitboxData = SearchHistory(Timestamp);
	if (!HitboxData) { return false; }

	const TArray<UBoxComponent*>& Hitboxes = FBCOwner->GetHitboxes();
	
	bIsRewinding = true;
	FillHitboxData(Hitboxes, PreRewindHitboxData);

	ApplyHitboxData(Hitboxes, *HitboxData);
	return true;
}

void ULagCompensationComponent::RestoreToPresent()
{
	if (!bIsRewinding)
	{
		UE_LOG(LogLagCompensation, Error, TEXT("Attempted to restore a rewind, but there is no active rewind"));
		return;
	}
	bIsRewinding = false;

	ApplyHitboxData(FBCOwner->GetHitboxes(), PreRewindHitboxData);
}

void ULagCompensationComponent::FillHitboxData(const TArray<UBoxComponent*>& Hitboxes, FHitboxData& OutData)
{
	OutData.Time = GameState->GetServerWorldTimeSeconds();
	for (int i = 0; i < MaxHitboxes && i < Hitboxes.Num(); ++i)
	{
		OutData.Hitboxes[i] = Hitboxes[i]->GetComponentTransform();
	}
}

void ULagCompensationComponent::ApplyHitboxData(const TArray<UBoxComponent*>& Hitboxes, const FHitboxData& HitboxData)
{
	for (int i = 0; i < MaxHitboxes && i < Hitboxes.Num(); ++i)
	{
		Hitboxes[i]->SetWorldTransform(HitboxData.Hitboxes[i]);
	}
}

const FHitboxData* ULagCompensationComponent::SearchHistory(float Timestamp) const
{
	if (PositionHistory.IsEmpty())
	{
		UE_LOG(LogLagCompensation, Warning, TEXT("Attempted to search with no history"));
		return nullptr;
	}
	if (PositionHistory.Last().Time > Timestamp)
	{
		UE_LOG(LogLagCompensation, Warning, TEXT("Received an expired timestamp"));
		return &PositionHistory.Last();
	}
	if (PositionHistory.First().Time < Timestamp)
	{
		UE_LOG(LogLagCompensation, Warning, TEXT("Received a timestamp ahead of current time"));
		return &PositionHistory.First();
	}

	// Pick the first datapoint that is older than the timestamp
	for (const auto& Data : PositionHistory)
	{
		if (Data.Time <= Timestamp)
		{
			return &Data;
		}
	}
	return nullptr;
}

FLagCompensatedWindow::FLagCompensatedWindow(const TArray<AFBCCharacter*>& Targets, double Timestamp)
{
	for (const auto Target : Targets)
	{
		ULagCompensationComponent* Comp = Target->GetLagCompensationComponent();
		if (!IsValid(Comp)) { return; }
		
		if (Comp->TryRewind(Timestamp))
		{
			RewindedTargets.Add(Comp);
		}
	}
}

FLagCompensatedWindow::~FLagCompensatedWindow()
{
	for (const auto Comp : RewindedTargets)
	{
		Comp->RestoreToPresent();
	}
}