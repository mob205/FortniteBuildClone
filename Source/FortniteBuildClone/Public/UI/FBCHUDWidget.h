// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayEffectTypes.h"
#include "Blueprint/UserWidget.h"
#include "Structure/Data/StructureResourceTypes.h"
#include "Component/BuildResourceComponent.h"
#include "Component/InventoryComponent.h"
#include "FBCHUDWidget.generated.h"

class UFBCAttributeSet;
class UFBCAbilitySystemComponent;
class AFBCPlayerState;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnResourceCountChangedSignature, EFBCResourceType, ChangedMaterialType, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedMulticastSignature, float, NewValue);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);

UCLASS()
class FORTNITEBUILDCLONE_API UFBCHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitializeHUD(AFBCPlayerState* PS, UFBCAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category = "Ability System")
	void AssignOnAttributeChanged(FGameplayAttribute Attribute, FOnAttributeChangedSignature Callback);
protected:
	UPROPERTY(BlueprintAssignable, Category = "Ability System")
	FOnResourceCountChangedSignature OnResourceCountChanged;

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

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UInventoryComponent> OwnerInventoryComponent;
	
	UFUNCTION(BlueprintCallable)
	void BroadcastInitialValues();
private:
	TObjectPtr<UFBCAttributeSet> AS;
	TMap<FGameplayAttribute, FOnAttributeChangedMulticastSignature> OnAttributeChangedMap;

	void OnAttributeChange(const FOnAttributeChangeData& OnAttributeChangeData);

	UFUNCTION()
	void BroadcastMaterialTypeChanged(EFBCResourceType NewMaterialType);
};
