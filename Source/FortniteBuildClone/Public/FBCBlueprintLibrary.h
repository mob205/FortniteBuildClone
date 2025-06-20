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
    static FVector SnapLocationToGrid_FloorZ(const FVector& InLocation);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
	static FVector SnapLocationToGrid_RoundZ(const FVector& InLocation);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
	static float SnapAngleToGrid(const float InAngle);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
	static int SnapAngleToGridInt(const float InAngle);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
	static FIntVector GetGridCoordinateLocation(FVector InWorldLocation);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Grid")
	static FIntVector GetCustomGridCoordinateLocation(FVector InWorldLocation, FVector InGridDimensions);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Ground Detection")
	static bool IsGround(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Trace")
	static bool TraceControllerLook(AController* PC, float Range, FHitResult& OutHit, ECollisionChannel TraceChannel = ECC_Visibility);
};
