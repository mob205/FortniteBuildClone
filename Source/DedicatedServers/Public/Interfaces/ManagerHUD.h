// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ManagerHUD.generated.h"

// This class does not need to be modified.
UINTERFACE(NotBlueprintable)
class UManagerHUD : public UInterface
{
	GENERATED_BODY()
};

class DEDICATEDSERVERS_API IManagerHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void OnSignIn() = 0;
};
