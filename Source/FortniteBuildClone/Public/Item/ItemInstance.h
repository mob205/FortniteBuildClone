// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Object.h"
#include "ItemInstance.generated.h"

class UItemData;
class UWidget;

USTRUCT(BlueprintType)
struct FItemInstance
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FInstancedStruct InstanceInfo{};

	UPROPERTY(BlueprintReadWrite)
	AActor* AssociatedActor{};
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<const UItemData> ItemData{};
};