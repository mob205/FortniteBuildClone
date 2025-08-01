// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/LagCompensationComponent.h"

#include "Components/BoxComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FBCCharacter.h"

ULagCompensationComponent::ULagCompensationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULagCompensationComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                              FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateHistory();
}

void ULagCompensationComponent::BeginPlay()
{
	Super::BeginPlay();

	FBCOwner = Cast<AFBCCharacter>(GetOwner());
	GameState = UGameplayStatics::GetGameState(this);
}

void ULagCompensationComponent::UpdateHistory()
{
	const float CurrentTime = GameState->GetServerWorldTimeSeconds();
	const TArray<UBoxComponent*>& Hitboxes = FBCOwner->GetHitboxes();

	// Push new data
	PositionHistory.PushFirst({});
	FHitboxData& Data = PositionHistory.First();

	Data.Time = CurrentTime;
	for (int i = 0; i < MaxHitboxes; ++i)
	{
		Data.Hitboxes[i] = Hitboxes[i]->GetComponentTransform();
	}

	// Clear expired data
	while (PositionHistory.Last().Time < CurrentTime - HistoryDuration)
	{
		PositionHistory.PopLast();
	}
}

