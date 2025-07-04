// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Test/APITestManager.h"
#include "GameplayTags/DedicatedServersTags.h"
#include "HTTPModule.h"
#include "JsonObjectConverter.h"
#include "Data/APIData.h"
#include "DedicatedServers/DedicatedServersLogs.h"
#include "Interfaces/IHttpResponse.h"
#include "UI/HTTP/HTTPRequestTypes.h"

void UAPITestManager::ListFleets()
{
	check(APIData);
	
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UAPITestManager::ListFleets_Response);

	const FString APIUrl = APIData->GetAPIEndpoint(DedicatedServersTags::GameSessionsAPI::ListFleets);
	Request->SetURL(APIUrl);
	Request->SetVerb("GET");
	Request->SetHeader("Content-Type", "application/json");
	Request->ProcessRequest();
}

void UAPITestManager::ListFleets_Response(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("List Fleets response received")));

	TSharedPtr<FJsonObject> JsonObject{};
	TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject))
	{

		if (ContainsError(JsonObject))
		{
			OnListFleetsResponseReceived.Broadcast({}, false);
			return;
		}
		
		if (JsonObject->HasField(TEXT("$metadata")))
		{
			TSharedPtr<FJsonObject> MetaDataJsonObject = JsonObject->GetObjectField(TEXT("$metadata"));
			FDSMetaData DSMetaData{};
			FJsonObjectConverter::JsonObjectToUStruct(MetaDataJsonObject.ToSharedRef(), &DSMetaData);

			DSMetaData.Dump();
		}

		FDSListFleetsResponse ListFleetsResponse{};
		FJsonObjectConverter::JsonObjectToUStruct(JsonObject.ToSharedRef(), &ListFleetsResponse);
		ListFleetsResponse.Dump();
		
		OnListFleetsResponseReceived.Broadcast(ListFleetsResponse, true);
	}
}
