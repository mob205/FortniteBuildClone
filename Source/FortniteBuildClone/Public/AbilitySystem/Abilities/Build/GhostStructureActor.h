// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GhostStructureActor.generated.h"

class APlayerController;

UCLASS(Abstract)
class FORTNITEBUILDCLONE_API AGhostStructureActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AGhostStructureActor();

	/**
	 *	Attempts to find a valid location to place a structure of a certain type.
	 *	@param PlayerController The player controller of the building player. Used to find facing direction, etc.
	 *	@param Range The maximum range to consider for valid locations
	 *	@param OutResult Out parameter to store the valid location, if there is one
	 *	@returns true if a valid location was found
	 */
	virtual bool GetTargetingLocation(APlayerController* PlayerController, float Range, FVector& OutResult) { return false; }

};
