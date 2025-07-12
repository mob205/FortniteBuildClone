// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DashboardWidget.h"

#include "DedicatedServers/DedicatedServersLogs.h"
#include "GameFramework/PlayerState.h"
#include "GameplayTags/APITags.h"
#include "HTTP/HTTPRequestManager.h"
#include "Kismet/GameplayStatics.h"
#include "Player/CredentialLocalPlayerSubsystem.h"

void UDashboardWidget::JoinGameSession()
{
	const FString* AccessToken{};
	APlayerController* PC = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (IsValid(PC))
	{
		if (auto* CredSubsystem = PC->GetLocalPlayer()->GetSubsystem<UCredentialLocalPlayerSubsystem>())
		{
			AccessToken = &CredSubsystem->GetAuthTokens().IdToken;
		}
	}
	
 	UHTTPRequestManager::StartAPIRequest(
	    APITags::FindOrCreateGameSession,
	    *GameSessionsAPIData,
	    FGameSessionResponse::StaticStruct(),
	    FOnResponseReceivedPayloadSignature::CreateUObject(this, &UDashboardWidget::OnGameSessionFound),
	    {},
	    AccessToken);	
	OnJoinStatusChanged.Broadcast("Searching for a game session...", true);
}

void UDashboardWidget::OnGameSessionFound(bool bWasSuccessful, FInstancedStruct&& InstancedResponse)
{
	FGameSessionResponse Response = InstancedResponse.Get<FGameSessionResponse>();
	if (!bWasSuccessful || Response.FleetId.IsEmpty())
	{
		OnJoinStatusChanged.Broadcast("An error occurred.", false);
		return;
	}
	HandleGameSessionStatus(Response);
}

void UDashboardWidget::HandleGameSessionStatus(const FGameSessionResponse& GameSessionInfo)
{
	if (GameSessionInfo.Status.Equals(TEXT("ACTIVE")))
	{
		OnJoinStatusChanged.Broadcast("Found a game session, creating player session...", false);
		TryCreatePlayerSession(GetPlayerUsername(), GameSessionInfo.GameSessionId);
	}
	else if (GameSessionInfo.Status.Equals(TEXT("ACTIVATING")))
	{
		FTimerDelegate CreateSessionDelegate = FTimerDelegate::CreateUObject(this, &UDashboardWidget::JoinGameSession);
		GetWorld()->GetTimerManager().SetTimer(WaitForSessionHandle, CreateSessionDelegate, 0.5f, false);
	}
	else
	{
		OnJoinStatusChanged.Broadcast("An error occurred.", false);
	}
}

FString UDashboardWidget::GetPlayerUsername() const
{
	APlayerController* PC = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (IsValid(PC))
	{
		if (auto* Subsystem = PC->GetLocalPlayer()->GetSubsystem<UCredentialLocalPlayerSubsystem>())
		{
			return Subsystem->GetUsername();
		}
	}
	return {};
}

void UDashboardWidget::TryCreatePlayerSession(const FString& PlayerID, const FString& GameSessionID)
{
	TMap<FString, FString> RequestContent = {
		{ "playerId", PlayerID},
		{ "gameSessionId", GameSessionID }
	};
	
	UHTTPRequestManager::StartAPIRequest(
		APITags::CreatePlayerSession,
		*GameSessionsAPIData,
		FPlayerSessionResponse::StaticStruct(),
		FOnResponseReceivedPayloadSignature::CreateUObject(this, &UDashboardWidget::OnPlayerSessionCreated),
		RequestContent
	);
}

void UDashboardWidget::OnPlayerSessionCreated(bool bWasSuccessful, FInstancedStruct&& InstancedResponse)
{
	if (!bWasSuccessful)
	{
		UE_LOG(LogDedicatedServers, Error, TEXT("PortalManager: Invalid player session created response"));
		return;
	}
	FPlayerSessionResponse Response = InstancedResponse.Get<FPlayerSessionResponse>();


	// Reset player input mode
	// Maybe do this when we actually get to the level?
	APlayerController* PC = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (IsValid(PC))
	{
		FInputModeGameOnly InputMode{};
		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(false);
	}

	const FString Options = "?PlayerSessionId=" + Response.PlayerSessionId + "?Username=" + Response.PlayerId;
	
	const FName Address = *(Response.IpAddress + TEXT(":") + FString::FromInt(Response.Port)); 
	UGameplayStatics::OpenLevel(this, Address, true, Options);
}