// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PortalManager.h"

#include "GameFramework/PlayerState.h"
#include "HTTP/HTTPRequestManager.h"

UPortalManager::UPortalManager()
{
	FindOrCreateGameSessionTag = FGameplayTag::RequestGameplayTag("API.FindOrCreateGameSession");
}

void UPortalManager::JoinGameSession()
{
 	HTTPManager->StartAPIRequest(
		FindOrCreateGameSessionTag,
		FGameSessionResponse::StaticStruct(),
		FOnResponseReceivedPayloadSignature::CreateUObject(this, &UPortalManager::OnGameSessionFound));	
	OnJoinStatusChanged.Broadcast("Searching for a game session...", true);
}

void UPortalManager::OnGameSessionFound(bool bWasSuccessful, FInstancedStruct InstancedResponse)
{
	FGameSessionResponse Response = InstancedResponse.Get<FGameSessionResponse>();
	if (!bWasSuccessful || Response.FleetId.IsEmpty())
	{
		OnJoinStatusChanged.Broadcast("An error occurred.", false);
		return;
	}
	HandleGameSessionStatus(Response);
}

void UPortalManager::HandleGameSessionStatus(const FGameSessionResponse& GameSessionInfo)
{
	if (GameSessionInfo.Status.Equals(TEXT("ACTIVE")))
	{
		OnJoinStatusChanged.Broadcast("Found a game session, creating player session...", false);
		TryCreatePlayerSession(GetUniquePlayerID(), GameSessionInfo.GameSessionId);
	}
	else if (GameSessionInfo.Status.Equals(TEXT("ACTIVATING")))
	{
		FTimerDelegate CreateSessionDelegate = FTimerDelegate::CreateUObject(this, &UPortalManager::JoinGameSession);
		GetWorld()->GetTimerManager().SetTimer(WaitForSessionHandle, CreateSessionDelegate, 0.5f, false);
	}
	else
	{
		OnJoinStatusChanged.Broadcast("An error occurred.", false);
	}
}

FString UPortalManager::GetUniquePlayerID() const
{
	APlayerController* PC = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (IsValid(PC))
	{
		APlayerState* PS = PC->GetPlayerState<APlayerState>();
		if (IsValid(PS))
		{
			return TEXT("Player_") + FString::FromInt(PS->GetUniqueID());
		}
	}
	return {};
}

void UPortalManager::TryCreatePlayerSession(const FString& PlayerID, const FString& GameSessionID)
{
	
}
