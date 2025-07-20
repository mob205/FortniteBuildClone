// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/EquippedItemActor.h"

#include "Player/FBCCharacter.h"

AEquippedItemActor::AEquippedItemActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SocketName = FName("RightHand");
}

void AEquippedItemActor::BeginPlay()
{
	Super::BeginPlay();

	SetActorHiddenInGame(true);
}

void AEquippedItemActor::OnItemEquipped(AFBCCharacter* AvatarActor)
{
	FAttachmentTransformRules TransformRules = { EAttachmentRule::SnapToTarget, false };
	AttachToActor(AvatarActor, TransformRules, SocketName);

	if (EquipMontage)
	{
		AvatarActor->PlayAnimMontage(EquipMontage);
	}
}

void AEquippedItemActor::OnItemUnequipped(AFBCCharacter* AvatarActor)
{
	
}
