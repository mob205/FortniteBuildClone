// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "APIData.generated.h"

UENUM(BlueprintType)
enum class EAPIVerb : uint8
{
	EAPIVerb_GET UMETA(DisplayName = "GET"),
	EAPIVerb_POST UMETA(DisplayName = "POST")
};

USTRUCT(BlueprintType)
struct FAPIEndpointDescription
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Endpoint{};

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	EAPIVerb Verb{};
};

UCLASS()
class DEDICATEDSERVERS_API UAPIData : public UDataAsset
{
	GENERATED_BODY()

public:
	UAPIData();
	
	FString GetAPIEndpoint(const FGameplayTag& EndpointTag);
	FString GetVerb(const FGameplayTag& EndpointTag);
	
protected:
	UPROPERTY(EditDefaultsOnly)
	FString Name;

	UPROPERTY(EditDefaultsOnly)
	FString InvokeURL;

	UPROPERTY(EditDefaultsOnly)
	FString Stage;

	UPROPERTY(EditDefaultsOnly)
	TMap<FGameplayTag, FAPIEndpointDescription> Resources{};

private:
	TMap<EAPIVerb, FString> Verbs{};
};
