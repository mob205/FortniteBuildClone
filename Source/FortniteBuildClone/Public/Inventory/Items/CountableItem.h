// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EquippedItemActor.h"
#include "UObject/Object.h"
#include "CountableItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCountChangedSignature, int32, Count);

UCLASS()
class FORTNITEBUILDCLONE_API ACountableItem : public AEquippedItemActor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnCountChangedSignature OnCountChanged;
	
	UFUNCTION(BlueprintCallable)
	void SetCount(uint8 NewCount);

	UFUNCTION(BlueprintCallable)
	uint8 GetCount() const { return Count;}
protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(ReplicatedUsing=OnRep_Count)
	uint8 Count{1};

private:
	UFUNCTION()
	void OnRep_Count();
};