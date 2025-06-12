// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FBCHUDWidget.h"

#include "AbilitySystem/FBCAbilitySystemComponent.h"
#include "AbilitySystem/FBCAttributeSet.h"
#include "Player/FBCPlayerState.h"

void UFBCHUDWidget::InitializeHUD(AFBCPlayerState* PS, UFBCAbilitySystemComponent* ASC)
{
	AbilitySystemComponent = ASC;

	AS = PS->GetAttributeSet();

	// Subscribe to material changes

	// Wood
	ASC->GetGameplayAttributeValueChangeDelegate(
		AS->GetWoodAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaterialCountChanged.Broadcast(EFBCMaterialType::FBCMat_Wood, Data.NewValue);
			}
	);

	// Brick
	ASC->GetGameplayAttributeValueChangeDelegate(
		AS->GetBrickAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaterialCountChanged.Broadcast(EFBCMaterialType::FBCMat_Brick, Data.NewValue);
			}
	);

	// Metal
	ASC->GetGameplayAttributeValueChangeDelegate(
		AS->GetMetalAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaterialCountChanged.Broadcast(EFBCMaterialType::FBCMat_Metal, Data.NewValue);
			}
	);
}

void UFBCHUDWidget::BroadcastInitialValues()
{
	OnMaterialCountChanged.Broadcast(EFBCMaterialType::FBCMat_Wood, AbilitySystemComponent->GetNumericAttribute(AS->GetWoodAttribute()));

	OnMaterialCountChanged.Broadcast(EFBCMaterialType::FBCMat_Brick, AbilitySystemComponent->GetNumericAttribute(AS->GetBrickAttribute()));

	OnMaterialCountChanged.Broadcast(EFBCMaterialType::FBCMat_Metal, AbilitySystemComponent->GetNumericAttribute(AS->GetMetalAttribute()));
}
