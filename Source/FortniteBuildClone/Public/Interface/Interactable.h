// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

class AFBCCharacter;
// This class does not need to be modified.
UINTERFACE()
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for interactables
 */
class FORTNITEBUILDCLONE_API IInteractable
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly)
	void StartInteract(AActor* Interactor);

	UFUNCTION(BlueprintNativeEvent, BlueprintAuthorityOnly)
	void StopInteract(AActor* Interactor);
};
