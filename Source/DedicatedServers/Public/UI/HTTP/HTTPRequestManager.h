// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "HTTPRequestManager.generated.h"

class UAPIData;
class FJsonObject;

UCLASS(Blueprintable)
class DEDICATEDSERVERS_API UHTTPRequestManager : public UObject
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAPIData> APIData;

	bool ContainsError(TSharedPtr<FJsonObject> JsonObject);
};
