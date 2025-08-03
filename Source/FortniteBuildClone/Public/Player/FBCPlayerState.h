// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "FBCPlayerState.generated.h"

class UInventoryComponent;
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
	UBuildResourceComponent* GetBuildResourceComponent() const { return BuildResourceComp; }
	
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UBuildResourceComponent> BuildResourceComp;
	
	UPROPERTY()
	TObjectPtr<UFBCAbilitySystemComponent> ASC;

	UPROPERTY()
	TObjectPtr<UFBCAttributeSet> AS;
};
