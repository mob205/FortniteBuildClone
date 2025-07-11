// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CredentialLocalPlayerSubsystem.h"

#include "DedicatedServers/DedicatedServersLogs.h"
#include "GameplayTags/APITags.h"
#include "HTTP/HTTPRequestManager.h"
#include "StructUtils/InstancedStruct.h"

void UCredentialLocalPlayerSubsystem::InitializeAuthTokens(const FAuthenticationResult& Result, const UAPIData* RefreshAPIData)
{
	AuthTokens = Result;

	APIData = RefreshAPIData;

	UWorld* World = GetWorld();
	if (IsValid(World))
	{
		FTimerDelegate RefreshDelegate = FTimerDelegate::CreateUObject(this, &UCredentialLocalPlayerSubsystem::RefreshTokens);
		World->GetTimerManager().SetTimer(RefreshTimerHandle, RefreshDelegate, TokenRefreshInterval, true);
	}
}

const FAuthenticationResult& UCredentialLocalPlayerSubsystem::GetAuthTokens() const
{
	return AuthTokens;
}

void UCredentialLocalPlayerSubsystem::RefreshTokens()
{
	FRefreshAuthenticationRequest Request{ AuthTokens.RefreshToken };
	FInstancedStruct InstancedRequest = FInstancedStruct::Make<FRefreshAuthenticationRequest>(Request);
	
	UHTTPRequestManager::StartAPIRequest(
		APITags::SignIn,
		*APIData,
		FSignInResponse::StaticStruct(),
		FOnResponseReceivedPayloadSignature::CreateUObject(this, &UCredentialLocalPlayerSubsystem::OnTokensRefreshed),
		&InstancedRequest);
}

void UCredentialLocalPlayerSubsystem::OnTokensRefreshed(bool bWasSuccessful, FInstancedStruct&& InstancedResponse)
{
	if (bWasSuccessful)
	{
		FSignInResponse& Response = InstancedResponse.GetMutable<FSignInResponse>();

		// Refresh token doesn't change and doesn't get sent back, so only update the other tokens
		AuthTokens.AccessToken = MoveTemp(Response.AuthenticationResult.AccessToken);
		AuthTokens.IdToken = MoveTemp(Response.AuthenticationResult.IdToken);

		UE_LOG(LogDedicatedServers, Log, TEXT("CredentialLocalPlayerSubsystem: Tokens successfully refreshed."));
		UE_LOG(LogDedicatedServers, Log, TEXT(" Access Token: %s"), *AuthTokens.AccessToken);
		UE_LOG(LogDedicatedServers, Log, TEXT(" IdToken: %s"), *AuthTokens.IdToken);
	}
	else
	{
		UE_LOG(LogDedicatedServers, Error, TEXT("CredentialLocalPlayerSubsystem: Failed to refresh tokens."));
	}
}
