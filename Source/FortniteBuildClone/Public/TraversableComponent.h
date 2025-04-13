// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TraversableComponent.generated.h"

class USplineComponent;
DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(bool, FIsValidLedgeSignature, USplineComponent*, Ledge);

UCLASS( BlueprintType, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FORTNITEBUILDCLONE_API UTraversableComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTraversableComponent();

	UFUNCTION(BlueprintCallable)
	void SetIsValidLedgeDelegate(FIsValidLedgeSignature IsValidLedgeDelegate) { CheckIsValidLedge = IsValidLedgeDelegate; }

protected:
	UFUNCTION(BlueprintCallable)
	bool IsValidLedge(USplineComponent* Ledge) const;

private:
	FIsValidLedgeSignature CheckIsValidLedge;
};
