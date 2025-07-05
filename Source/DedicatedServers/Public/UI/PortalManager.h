// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "APIWidgetBase.h"
#include "PortalManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnJoinStatusChangedSignature, const FString&, StatusMessage);

UCLASS()
class DEDICATEDSERVERS_API UPortalManager : public UAPIWidgetBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnJoinStatusChangedSignature OnJoinStatusChanged;

	UFUNCTION(BlueprintCallable)
	void JoinGameSession();
};
