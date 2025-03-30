// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FBCBlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class FORTNITEBUILDCLONE_API UFBCBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
	static FTransform SnapTransformToGrid(const FTransform& InTransform);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
    static FRotator SnapRotationToGrid(const FRotator& InRotator);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
    static FVector SnapLocationToGrid(const FVector& InLocation);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
	static float SnapAngleToGrid(const float InAngle);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
	static FIntVector GetGridCoordinateLocation(FVector InWorldLocation);
};
