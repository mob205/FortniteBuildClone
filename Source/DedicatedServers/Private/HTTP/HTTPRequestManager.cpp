// Fill out your copyright notice in the Description page of Project Settings.


#include "HTTP/HTTPRequestManager.h"

#include "GameplayTagContainer.h"
#include "HTTPModule.h"
#include "JsonObjectConverter.h"
#include "Data/APIData.h"
#include "DedicatedServers/DedicatedServersLogs.h"
#include "Interfaces/IHttpResponse.h"
#include "StructUtils/InstancedStruct.h"


void UHTTPRequestManager::StartAPIRequest(FGameplayTag EndpointTag, FInstancedStruct& OutputStruct, FOnResponseReceivedSignature Callback)
{
	check(APIData);

	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindLambda([this, Callback, &OutputStruct]
		(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
			bool bResult = ParseResponse(Request, Response, bWasSuccessful, OutputStruct);
			Callback.ExecuteIfBound(bResult);
		});

	const FString APIUrl = APIData->GetAPIEndpoint(EndpointTag);
	Request->SetURL(APIUrl);
	Request->SetVerb("GET");
	Request->SetHeader("Content-Type", "application/json");
	Request->ProcessRequest();
}

bool UHTTPRequestManager::ParseResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FInstancedStruct& OutResult)
{
	if (!bWasSuccessful)
	{
		return false;
	}
	
	TSharedPtr<FJsonObject> JsonObject{};
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

	if (!FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{
		UE_LOG(LogDedicatedServers, Error, TEXT("HTTPRequestManager: Could not deserialize json"));
		return false;
	}

	if (ContainsError(JsonObject))
	{
		UE_LOG(LogDedicatedServers, Error, TEXT("HTTPRequestManager: Request response is error"))
		return false;
	}

	return FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), OutResult.GetScriptStruct(), OutResult.GetMutableMemory());
}

bool UHTTPRequestManager::ContainsError(TSharedPtr<FJsonObject> JsonObject)
{
	// Check for common error fields
	if (JsonObject->HasField(TEXT("errorType")) || JsonObject->HasField(TEXT("errorMessage")))
	{
		FString ErrorType = JsonObject->HasField(TEXT("errorType")) ? JsonObject->GetStringField(TEXT("errorType")) : TEXT("Unknown Error");
		FString ErrorMessage = JsonObject->HasField(TEXT("errorMessage")) ? JsonObject->GetStringField(TEXT("errorMessage")) : TEXT("Unknown Error Message");

		UE_LOG(LogDedicatedServers, Error, TEXT("Error Type: %s"), *ErrorType);
		UE_LOG(LogDedicatedServers, Error, TEXT("Error Message: %s"), *ErrorMessage);

		return true;
	}
	if (JsonObject->HasField(TEXT("$fault")))
	{
		FString ErrorType = JsonObject->HasField(TEXT("name")) ? JsonObject->GetStringField(TEXT("name")) : TEXT("Unknown Error");

		UE_LOG(LogDedicatedServers, Error, TEXT("Error Type: %s"), *ErrorType);
		return true;
	}
	return false;
}