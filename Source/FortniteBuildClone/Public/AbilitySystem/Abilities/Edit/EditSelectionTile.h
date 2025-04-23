// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EditSelectionTile.generated.h"

UCLASS(BlueprintType, Blueprintable)
class FORTNITEBUILDCLONE_API AEditSelectionTile : public AActor
{
	GENERATED_BODY()
	
public:	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetSelection(bool bIsSelected);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void SetValidity(bool bIsValidEdit);

	UFUNCTION(BlueprintCallable)
	int GetEditBitfieldIndex() const { return EditBitfieldIndex;}

	UFUNCTION(BlueprintCallable)
	void SetEditBitfieldIndex(int NewIndex) { EditBitfieldIndex = NewIndex; }
protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	int EditBitfieldIndex{};
};
