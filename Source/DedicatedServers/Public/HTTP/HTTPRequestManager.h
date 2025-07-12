// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Interfaces/IHttpRequest.h"

struct FInstancedStruct;
class UAPIData;
class FJsonObject;

DECLARE_DELEGATE_OneParam(FOnResponseReceivedSignature, bool);
DECLARE_DELEGATE_TwoParams(FOnResponseReceivedPayloadSignature, bool, FInstancedStruct&&);

class DEDICATEDSERVERS_API UHTTPRequestManager
{
public:
	static void StartAPIRequest(const FGameplayTag& EndpointTag, const UAPIData& APIData,
		UScriptStruct* StructType, FOnResponseReceivedPayloadSignature Callback, const FInstancedStruct* RequestBody = nullptr,
		const FString* AccessToken = nullptr);
	
	static void StartAPIRequest(const FGameplayTag& EndpointTag, const UAPIData& APIData,
		FInstancedStruct& OutputStruct, FOnResponseReceivedSignature Callback, const FString* AccessToken = nullptr);
	
private:
	static bool ContainsError(TSharedPtr<FJsonObject> JsonObject);
	
	static bool ParseResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FInstancedStruct& OutResult);

	static void StartAPIRequestInternal(TSharedRef<IHttpRequest> Request, const FGameplayTag& EndpointTag,
		const UAPIData& APIData, const FInstancedStruct* RequestBody = nullptr, const FString* AccessToken = nullptr);
	
	static bool GetRequestString(const FInstancedStruct& RequestBody, FString& OutString);
};
