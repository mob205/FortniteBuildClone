// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "DedicatedServerGameMode.generated.h"

struct FServerParameters;
struct FProcessParameters;
DECLARE_LOG_CATEGORY_EXTERN(LogDedicatedServerGameMode, Log, All);

UCLASS()
class DEDICATEDSERVERS_API ADedicatedServerGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	// Configure server parameters for GameLift Anywhere
	void ConfigureServerParameters(FServerParameters& ServerParametersForAnywhere);
	int32 GetPort();

private:
	TSharedPtr<FProcessParameters> ProcessParameters{};

	void InitGameLift();
};
