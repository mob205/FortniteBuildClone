// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/APIResponseTypes.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "CredentialLocalPlayerSubsystem.generated.h"

struct FInstancedStruct;
class UAPIData;

UCLASS(BlueprintType)
class DEDICATEDSERVERS_API UCredentialLocalPlayerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void InitializeAuthTokens(const FAuthenticationResult& Result, const UAPIData* RefreshAPIData);

protected:
	float TokenRefreshInterval = 2700.f;

private:
	void RefreshTokens();

	void OnTokensRefreshed(bool bWasSuccessful, FInstancedStruct&& Response);

	TObjectPtr<const UAPIData> APIData{};

	FAuthenticationResult AuthTokens;
	
	FTimerHandle RefreshTimerHandle;
};
