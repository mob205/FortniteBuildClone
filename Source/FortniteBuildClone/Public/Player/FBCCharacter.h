// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "FBCCharacter.generated.h"

class UFBCAttributeSet;
class UFBCAbilitySystemComponent;
class UGameplayAbility;

UCLASS()
class FORTNITEBUILDCLONE_API AFBCCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	
	
private:
	void InitAbilityActorInfo();

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayAbility>> InitialAbilities;

	void GrantInitialAbilities();

	UPROPERTY()
	TObjectPtr<UFBCAbilitySystemComponent> ASC;

	UPROPERTY()
	TObjectPtr<UFBCAttributeSet> AS;
};
