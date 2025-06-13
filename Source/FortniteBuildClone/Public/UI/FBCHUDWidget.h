// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/StructureMaterialTypes.h"
#include "FBCHUDWidget.generated.h"

class UFBCAttributeSet;
class UFBCAbilitySystemComponent;
class AFBCPlayerState;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaterialCountChanged, EFBCMaterialType, ChangedMaterialType, float, NewValue);
/**
 * 
 */
UCLASS()
class FORTNITEBUILDCLONE_API UFBCHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeHUD(AFBCPlayerState* PS, UFBCAbilitySystemComponent* ASC);

protected:
	UPROPERTY(BlueprintAssignable, Category = "Ability System")
	FOnMaterialCountChanged OnMaterialCountChanged;
	
	UPROPERTY(BlueprintReadOnly, Category = "Ability System")
	UFBCAbilitySystemComponent* AbilitySystemComponent;

	UFUNCTION(BlueprintCallable)
	void BroadcastInitialValues();
private:
	UFBCAttributeSet* AS;
};
