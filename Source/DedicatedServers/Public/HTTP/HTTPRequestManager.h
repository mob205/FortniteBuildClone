// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Interfaces/IHttpRequest.h"
#include "UI/Test/APITestOverlay.h"

#include "HTTPRequestManager.generated.h"

struct FInstancedStruct;
class UAPIData;
class FJsonObject;

DECLARE_DELEGATE_OneParam(FOnResponseReceivedSignature, bool);

UCLASS(Blueprintable, BlueprintType)
class DEDICATEDSERVERS_API UHTTPRequestManager : public UObject
{
	GENERATED_BODY()

public:
	void StartAPIRequest(FGameplayTag EndpointTag, FInstancedStruct& OutputStruct, FOnResponseReceivedSignature Callback);
	
	void SetAPIData(UAPIData* Data) { APIData = Data; }

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAPIData> APIData;

private:
	bool ContainsError(TSharedPtr<FJsonObject> JsonObject);
	
	bool ParseResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful, FInstancedStruct& OutResult);

};
