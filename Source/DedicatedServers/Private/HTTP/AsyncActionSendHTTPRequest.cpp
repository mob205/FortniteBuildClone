// Fill out your copyright notice in the Description page of Project Settings.


#include "HTTP/AsyncActionSendHTTPRequest.h"

#include "HTTP/HTTPRequestManager.h"
#include "StructUtils/InstancedStruct.h"


UAsyncActionSendHTTPRequest* UAsyncActionSendHTTPRequest::AsyncSendAPIRequestWithContent(UObject* WorldContextObject,
	FGameplayTag EndpointTag, const UAPIData* APIData, UScriptStruct* OutputStructType,
	const TMap<FString, FString>& RequestContent, const FString& AccessToken)
{
	UAsyncActionSendHTTPRequest* Action = AsyncSendAPIRequest(WorldContextObject, EndpointTag, APIData, OutputStructType, AccessToken);
	Action->RequestContent = RequestContent;
	
	return Action;
}

UAsyncActionSendHTTPRequest* UAsyncActionSendHTTPRequest::AsyncSendAPIRequest(
	UObject* WorldContextObject, FGameplayTag EndpointTag, const UAPIData* APIData, UScriptStruct* OutputStructType, const FString& AccessToken)
{
	UAsyncActionSendHTTPRequest* Action = NewObject<UAsyncActionSendHTTPRequest>();
	Action->RegisterWithGameInstance(WorldContextObject);
	
	Action->OutputStructType = Cast<UScriptStruct>(OutputStructType);
	Action->EndpointTag = EndpointTag;
	Action->APIData = APIData;

	if (!AccessToken.IsEmpty())
	{
		Action->AccessToken = &AccessToken;
	}
	
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
		RequestContent,
		AccessToken);
}

void UAsyncActionSendHTTPRequest::OnResponseReceived(bool bWasSuccessful, FInstancedStruct&& Result)
{
	OnCompleted.Broadcast(bWasSuccessful, MoveTemp(Result));

	SetReadyToDestroy();
	MarkAsGarbage();
}
