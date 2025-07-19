// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Object.h"
#include "ItemSlot.generated.h"

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
	TObjectPtr<UItemData> ItemData{};
};

UCLASS()
class FORTNITEBUILDCLONE_API UItemSlot : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(const FItemInstance& InItemInstance)
	{
		ItemInstance = InItemInstance;
	}

	void Initialize(FItemInstance&& InItemInstance)
	{
		ItemInstance = InItemInstance;
	}
	
protected:
	UPROPERTY(BlueprintReadWrite)
	FItemInstance ItemInstance;
};
