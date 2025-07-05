// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
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
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true"))
	static UAsyncActionSendHTTPRequest* AsyncSendAPIRequest(UHTTPRequestManager* HTTPManager, FGameplayTag EndpointTag, UStruct* StructType);

	UPROPERTY(BlueprintAssignable)
	FOnRequestCompleteSignature OnCompleted;
	
	virtual void Activate() override;

private:
	FGameplayTag EndpointTag;

	TObjectPtr<UHTTPRequestManager> HTTPManager{};

	FInstancedStruct Result{};

	void OnResponseReceived(bool bWasSuccessful);
};
