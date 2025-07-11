// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Data/APIData.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "StructUtils/InstancedStruct.h"
#include "AsyncActionSendHTTPRequest.generated.h"

class UHTTPRequestManager;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRequestCompleteSignature, bool, bWasSuccessful, FInstancedStruct, Result);

UCLASS()
class DEDICATEDSERVERS_API UAsyncActionSendHTTPRequest : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"))
	static UAsyncActionSendHTTPRequest* AsyncSendAPIRequestWithContent(UObject* WorldContextObject, FGameplayTag EndpointTag, const UAPIData* APIData, UScriptStruct* OutputStructType, const
	                                                                   FInstancedStruct& RequestContent, const FString& AccessToken);
	
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true"))
	static UAsyncActionSendHTTPRequest* AsyncSendAPIRequest(UObject* WorldContextObject, FGameplayTag EndpointTag, const UAPIData* APIData, UScriptStruct* OutputStructType, const FString& AccessToken);

	UPROPERTY(BlueprintAssignable)
	FOnRequestCompleteSignature OnCompleted;
	
	virtual void Activate() override;

private:
	FGameplayTag EndpointTag;

	TObjectPtr<const UAPIData> APIData{};

	UScriptStruct* OutputStructType{};
	
	const FInstancedStruct* InputStruct{};

	const FString* AccessToken{};
	
	void OnResponseReceived(bool bWasSuccessful, FInstancedStruct&& Result);
};
