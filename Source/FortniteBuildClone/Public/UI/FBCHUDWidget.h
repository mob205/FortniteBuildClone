// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/StructureMaterialTypes.h"
#include "Interface/MaterialSwitchable.h"
#include "FBCHUDWidget.generated.h"

class UFBCAttributeSet;
class UFBCAbilitySystemComponent;
class AFBCPlayerState;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaterialCountChangedSignature, EFBCMaterialType, ChangedMaterialType, float, NewValue);
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
	FOnMaterialCountChangedSignature OnMaterialCountChanged;

	UPROPERTY(BlueprintAssignable, Category = "Material")
	FOnMaterialTypeChangedMulticastSignature OnMaterialTypeChanged;
	
	UPROPERTY(BlueprintReadOnly, Category = "Ability System")
	UFBCAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Ability System")
	AActor* Avatar;

	UFUNCTION(BlueprintCallable)
	void BroadcastInitialValues();
private:
	UFBCAttributeSet* AS;

	UFUNCTION()
	void BroadcastMaterialTypeChanged(EFBCMaterialType NewMaterialType);
};
