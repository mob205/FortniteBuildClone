// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FBCCharacterBase.h"
#include "FBCAICharacter.generated.h"

UCLASS()
class FORTNITEBUILDCLONE_API AFBCAICharacter : public AFBCCharacterBase
{
	GENERATED_BODY()

public:
	AFBCAICharacter();

protected:
	virtual void InitAbilityActorInfo() override;
};
