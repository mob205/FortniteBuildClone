// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/General/CountableItem.h"

#include "FortniteBuildClone/FortniteBuildClone.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

void ACountableItem::SetCount(uint8 NewCount)
{
	if (NewCount != Count)
	{
		Count = NewCount;
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, Count, this);
		OnCountChanged.Broadcast(Count);
	}
}

void ACountableItem::OnRep_Count()
{
	OnCountChanged.Broadcast(Count);
	if (HasAuthority())
	{
		UE_LOG(LogFBC, Error, TEXT("RepNotify on server??"));
	}
}

void ACountableItem::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	DOREPLIFETIME_WITH_PARAMS_FAST(ACountableItem, Count, Params);
}
