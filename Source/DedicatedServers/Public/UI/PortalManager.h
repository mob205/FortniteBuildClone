// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "APIWidgetBase.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "Data/APIResponseTypes.h"
#include "PortalManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnJoinStatusChangedSignature, const FString&, StatusMessage, bool, bIsJoining);

UCLASS()
class DEDICATEDSERVERS_API UPortalManager : public UAPIWidgetBase
{
	GENERATED_BODY()

public:
	UPortalManager();
	
	UPROPERTY(BlueprintAssignable)
	FOnJoinStatusChangedSignature OnJoinStatusChanged;

	UFUNCTION(BlueprintCallable)
	void JoinGameSession();

private:
	FGameplayTag FindOrCreateGameSessionTag;
	FGameSessionResponse APIResponse{};

	FString GetUniquePlayerID() const;

	void HandleGameSessionStatus(const FGameSessionResponse& GameSessionInfo);
	
	void OnGameSessionFound(bool bWasSuccessful, FInstancedStruct Response);

	void TryCreatePlayerSession(const FString& PlayerID, const FString& GameSessionID);

	FTimerHandle WaitForSessionHandle{};
};
