// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Traversable.generated.h"

class USplineComponent;

UINTERFACE(BlueprintType)
class UTraversable : public UInterface
{
	GENERATED_BODY()
};


class FORTNITEBUILDCLONE_API ITraversable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	const TArray<USplineComponent*> GetLedges() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	const TMap<USplineComponent*, USplineComponent*> GetOppositeLedges() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsValidLedge(USplineComponent* Ledge) const;
};
