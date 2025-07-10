#pragma once

#pragma once

#include "CoreMinimal.h"
#include "APIResponseTypes.generated.h"

USTRUCT()
struct FGameSessionResponse
{
	GENERATED_BODY()

	UPROPERTY()
	double CreationTime{};

	UPROPERTY()
	FString CreatorId{};

	UPROPERTY()
	int32 CurrentPlayerSessionCount{};

	UPROPERTY()
	FString DnsName{};

	UPROPERTY()
	FString FleetArn{};

	UPROPERTY()
	FString FleetId{};

	UPROPERTY()
	TMap<FString, FString> GameProperties{};

	UPROPERTY()
	FString GameSessionData{};

	UPROPERTY()
	FString GameSessionId{};

	UPROPERTY()
	FString IpAddress{};

	UPROPERTY()
	FString Location{};
	
	UPROPERTY()
	FString MatchmakerData{};

	UPROPERTY()
	int32 MaximumPlayerSessionCount{};

	UPROPERTY()
	FString Name{};

	UPROPERTY()
	FString PlayerSessionCreationPolicy{};

	UPROPERTY()
	int32 Port{};

	UPROPERTY()
	FString Status{};

	UPROPERTY()
	FString StatusReason{};

	UPROPERTY()
	double TerminationTime{};
};

USTRUCT()
struct FPlayerSessionRequest
{
	GENERATED_BODY()
	
	UPROPERTY()
	FString PlayerId{};

	UPROPERTY()
	FString GameSessionId{};
};

USTRUCT()
struct FPlayerSessionResponse
{
	GENERATED_BODY()

	UPROPERTY()
	FString CreationTime{};

	UPROPERTY()
	FString DnsName{};

	UPROPERTY()
	FString FleetArn{};

	UPROPERTY()
	FString FleetId{};

	UPROPERTY()
	FString GameSessionId{};

	UPROPERTY()
	FString IpAddress{};

	UPROPERTY()
	FString PlayerData{};

	UPROPERTY()
	FString PlayerId{};

	UPROPERTY()
	FString PlayerSessionId{};

	UPROPERTY()
	int32 Port{};

	UPROPERTY()
	FString Status{};

	UPROPERTY()
	FString TerminationTime{};
};

USTRUCT(BlueprintType)
struct FCodeDeliveryDetails
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString AttributeName{};

	UPROPERTY(BlueprintReadWrite)
	FString DeliveryMedium{};

	UPROPERTY(BlueprintReadWrite)
	FString Destination{};
};

USTRUCT(BlueprintType)
struct FSignUpResponse
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	FCodeDeliveryDetails CodeDeliveryDetails{};

	UPROPERTY(BlueprintReadWrite)
	FString Session{};
	
	UPROPERTY(BlueprintReadWrite)
	bool UserConfirmed{};

	UPROPERTY(BlueprintReadWrite)
	FString UserSub{};
};

USTRUCT(BlueprintType)
struct FSignInRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Username{};

	UPROPERTY(BlueprintReadWrite)
	FString Password{};
};

USTRUCT(BlueprintType)
struct FNewDeviceMetaData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString DeviceGroupKey{};

	UPROPERTY(BlueprintReadWrite)
	FString DeviceKey{};
};

USTRUCT(BlueprintType)
struct FAuthenticationResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString AccessToken{};

	UPROPERTY(BlueprintReadWrite)
	double ExpiresIn{};

	UPROPERTY(BlueprintReadWrite)
	FString IdToken{};

	UPROPERTY(BlueprintReadWrite)
	FNewDeviceMetaData NewDeviceMetaData{};

	UPROPERTY(BlueprintReadWrite)
	FString RefreshToken{};

	UPROPERTY(BlueprintReadWrite)
	FString TokenType{};
};

USTRUCT(BlueprintType)
struct FSignInResponse
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FAuthenticationResult AuthenticationResult{};
	
	UPROPERTY(BlueprintReadWrite)
	TArray<FString> AvailableChallenges{};

	UPROPERTY(BlueprintReadWrite)
	FString ChallengeName{};
	
	UPROPERTY(BlueprintReadWrite)
	TMap<FString, FString> ChallengeParameters{};

	UPROPERTY(BlueprintReadWrite)
	FString Session{};
};

USTRUCT(BlueprintType)
struct FConfirmSignUpRequest
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Username{};

	UPROPERTY(BlueprintReadWrite)
	FString ConfirmationCode{};
};

USTRUCT()
struct FRefreshAuthenticationRequest
{
	GENERATED_BODY()

	UPROPERTY()
	FString RefreshToken;
};
