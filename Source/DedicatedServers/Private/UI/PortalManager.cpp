// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PortalManager.h"

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
	}
	else
	{
		OnJoinStatusChanged.Broadcast("Found a game session!", true);
	}
}

