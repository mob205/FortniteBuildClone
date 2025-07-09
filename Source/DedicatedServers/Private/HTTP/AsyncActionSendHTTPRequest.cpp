// Fill out your copyright notice in the Description page of Project Settings.


#include "HTTP/AsyncActionSendHTTPRequest.h"

#include "DedicatedServers/DedicatedServersLogs.h"
#include "HTTP/HTTPRequestManager.h"
#include "StructUtils/InstancedStruct.h"


UAsyncActionSendHTTPRequest* UAsyncActionSendHTTPRequest::AsyncSendAPIRequestWithContent(UObject* WorldContextObject,
	FGameplayTag EndpointTag, const UAPIData* APIData, UStruct* OutputStructType, const FInstancedStruct& RequestContent)
{
	UAsyncActionSendHTTPRequest* Action = AsyncSendAPIRequest(WorldContextObject, EndpointTag, APIData, OutputStructType);
	Action->InputStruct = &RequestContent;
	
	return Action;
}

UAsyncActionSendHTTPRequest* UAsyncActionSendHTTPRequest::AsyncSendAPIRequest(
	UObject* WorldContextObject, FGameplayTag EndpointTag, const UAPIData* APIData, UStruct* OutputStructType)
{
	UAsyncActionSendHTTPRequest* Action = NewObject<UAsyncActionSendHTTPRequest>();
	Action->RegisterWithGameInstance(WorldContextObject);

	// We need our data to be outputted as a FInstancedStruct for compatibility with Blueprints, which uses UScriptStruct,
	// but the Blueprint dropdown for UStruct is nicer than UScriptStruct
	Action->OutputStructType = Cast<UScriptStruct>(OutputStructType);
	Action->EndpointTag = EndpointTag;
	Action->APIData = APIData;
	
	return Action;
}

void UAsyncActionSendHTTPRequest::Activate()
{
	Super::Activate();

	UHTTPRequestManager::StartAPIRequest(
		EndpointTag,
		*APIData,
		OutputStructType,
		FOnResponseReceivedPayloadSignature::CreateUObject(this, &UAsyncActionSendHTTPRequest::OnResponseReceived),
		InputStruct);
}

void UAsyncActionSendHTTPRequest::OnResponseReceived(bool bWasSuccessful, FInstancedStruct&& Result)
{
	OnCompleted.Broadcast(bWasSuccessful, MoveTemp(Result));

	SetReadyToDestroy();
	MarkAsGarbage();
}
