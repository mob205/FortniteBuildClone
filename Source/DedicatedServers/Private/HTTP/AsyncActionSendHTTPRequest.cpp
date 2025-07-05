// Fill out your copyright notice in the Description page of Project Settings.


#include "HTTP/AsyncActionSendHTTPRequest.h"

#include "DedicatedServers/DedicatedServersLogs.h"
#include "HTTP/HTTPRequestManager.h"
#include "StructUtils/InstancedStruct.h"


UAsyncActionSendHTTPRequest* UAsyncActionSendHTTPRequest::AsyncSendAPIRequest(UHTTPRequestManager* HTTPManager, FGameplayTag EndpointTag, UStruct* StructType)
{
	UAsyncActionSendHTTPRequest* Action = NewObject<UAsyncActionSendHTTPRequest>();
	Action->RegisterWithGameInstance(HTTPManager);

	// We need our data to be outputted as a FInstancedStruct for compatibility with Blueprints, which uses UScriptStruct,
	// but the Blueprint dropdown for UStruct is nicer than UScriptStruct
	Action->Result.InitializeAs(Cast<UScriptStruct>(StructType), nullptr);
	Action->EndpointTag = EndpointTag;
	Action->HTTPManager = HTTPManager;
	
	return Action;
}

void UAsyncActionSendHTTPRequest::Activate()
{
	Super::Activate();

	HTTPManager->StartAPIRequest(
		EndpointTag,
		Result,
		FOnResponseReceivedSignature::CreateUObject(this, &UAsyncActionSendHTTPRequest::OnResponseReceived));
}

void UAsyncActionSendHTTPRequest::OnResponseReceived(bool bWasSuccessful)
{
	OnCompleted.Broadcast(bWasSuccessful, Result);

	SetReadyToDestroy();
	MarkAsGarbage();
}
