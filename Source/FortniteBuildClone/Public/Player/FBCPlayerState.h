// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FBCPlayerState.generated.h"

class UFBCAbilitySystemComponent;
class UFBCAttributeSet;
class UBuildResourceComponent;

/**
 * 
 */
UCLASS()
class FORTNITEBUILDCLONE_API AFBCPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AFBCPlayerState();

	UFBCAbilitySystemComponent* GetAbilitySystemComponent() const { return ASC; }
	UFBCAttributeSet* GetAttributeSet() const { return AS; }
	
protected:
	UPROPERTY()
	TObjectPtr<UFBCAbilitySystemComponent> ASC;

	UPROPERTY()
	TObjectPtr<UFBCAttributeSet> AS;

	UPROPERTY()
	TObjectPtr<UBuildResourceComponent> BuildResourceComp;
};
