// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EquippedItemActor.generated.h"

class AFBCCharacter;

UCLASS()
class FORTNITEBUILDCLONE_API AEquippedItemActor : public AActor
{
	GENERATED_BODY()

public:
	AEquippedItemActor();
	
	virtual void OnItemEquipped(AFBCCharacter* AvatarActor);
	virtual void OnItemUnequipped(AFBCCharacter* AvatarActor);

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly)
	FName SocketName{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> EquipMontage{};
};

