// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Data/StructureResourceTypes.h"
#include "Component/BuildResourceComponent.h"
#include "FBCHUDWidget.generated.h"

class UFBCAttributeSet;
class UFBCAbilitySystemComponent;
class AFBCPlayerState;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMaterialCountChangedSignature, EFBCResourceType, ChangedMaterialType, float, NewValue);
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
	FOnResourceTypeChangedSignature OnResourceTypeChanged;
	
	UPROPERTY(BlueprintReadOnly, Category = "Ability System")
	UFBCAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Ability System")
	TObjectPtr<AActor> Avatar;

	UPROPERTY(BlueprintReadOnly, Category = "Ability System")
	TObjectPtr<AActor> Owner;

	UPROPERTY(BlueprintReadOnly, Category = "Ability System")
	TObjectPtr<UBuildResourceComponent> OwnerResourceComponent;

	UFUNCTION(BlueprintCallable)
	void BroadcastInitialValues();
private:
	TObjectPtr<UFBCAttributeSet> AS;

	UFUNCTION()
	void BroadcastMaterialTypeChanged(EFBCResourceType NewMaterialType);
};
