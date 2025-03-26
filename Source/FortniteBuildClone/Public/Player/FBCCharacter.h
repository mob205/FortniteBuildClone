// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FBCCharacter.generated.h"

class UFBCAttributeSet;
class UFBCAbilitySystemComponent;

UCLASS()
class FORTNITEBUILDCLONE_API AFBCCharacter : public ACharacter
{
	GENERATED_BODY()

protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

private:
	void InitAbilityActorInfo();

	UPROPERTY()
	TObjectPtr<UFBCAbilitySystemComponent> ASC;

	UPROPERTY()
	TObjectPtr<UFBCAttributeSet> AS;
};
