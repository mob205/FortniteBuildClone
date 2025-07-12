// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StructUtils/InstancedStruct.h"
#include "Data/APIResponseTypes.h"
#include "DashboardWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnJoinStatusChangedSignature, const FString&, StatusMessage, bool, bIsJoining);

class UAPIData;

UCLASS()
class DEDICATEDSERVERS_API UDashboardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintAssignable)
	FOnJoinStatusChangedSignature OnJoinStatusChanged;

	UFUNCTION(BlueprintCallable)
	void JoinGameSession();

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAPIData> GameSessionsAPIData{};
	
private:
	FString GetUniquePlayerID() const;

	void HandleGameSessionStatus(const FGameSessionResponse& GameSessionInfo);
	
	void OnGameSessionFound(bool bWasSuccessful, FInstancedStruct&& InstancedResponse);

	void TryCreatePlayerSession(const FString& PlayerID, const FString& GameSessionID);

	void OnPlayerSessionCreated(bool bWasSuccessful, FInstancedStruct&& InstancedResponse);
	
	FTimerHandle WaitForSessionHandle{};
};
