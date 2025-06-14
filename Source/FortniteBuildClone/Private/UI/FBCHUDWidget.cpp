// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FBCHUDWidget.h"

#include "AbilitySystem/FBCAbilitySystemComponent.h"
#include "AbilitySystem/FBCAttributeSet.h"
#include "Player/FBCPlayerState.h"

void UFBCHUDWidget::InitializeHUD(AFBCPlayerState* PS, UFBCAbilitySystemComponent* ASC)
{
	AbilitySystemComponent = ASC;
	AS = PS->GetAttributeSet();
	Avatar = ASC->GetAvatarActor();
	Owner = ASC->GetOwnerActor();
	if (Owner)
	{
		OwnerResourceComponent = Owner->GetComponentByClass<UBuildResourceComponent>();
	}
	
	// Subscribe to material changes

	// Wood
	ASC->GetGameplayAttributeValueChangeDelegate(
		AS->GetWoodAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaterialCountChanged.Broadcast(EFBCResourceType::FBCMat_Wood, Data.NewValue);
			}
	);

	// Brick
	ASC->GetGameplayAttributeValueChangeDelegate(
		AS->GetBrickAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaterialCountChanged.Broadcast(EFBCResourceType::FBCMat_Brick, Data.NewValue);
			}
	);

	// Metal
	ASC->GetGameplayAttributeValueChangeDelegate(
		AS->GetMetalAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaterialCountChanged.Broadcast(EFBCResourceType::FBCMat_Metal, Data.NewValue);
			}
	);

	if (OwnerResourceComponent)
	{
		OwnerResourceComponent->OnResourceTypeChanged.AddDynamic(this, &UFBCHUDWidget::BroadcastMaterialTypeChanged);
	}
}

void UFBCHUDWidget::BroadcastInitialValues()
{
	OnMaterialCountChanged.Broadcast(EFBCResourceType::FBCMat_Wood, AbilitySystemComponent->GetNumericAttribute(AS->GetWoodAttribute()));

	OnMaterialCountChanged.Broadcast(EFBCResourceType::FBCMat_Brick, AbilitySystemComponent->GetNumericAttribute(AS->GetBrickAttribute()));

	OnMaterialCountChanged.Broadcast(EFBCResourceType::FBCMat_Metal, AbilitySystemComponent->GetNumericAttribute(AS->GetMetalAttribute()));

	if (OwnerResourceComponent)
	{
		OnResourceTypeChanged.Broadcast(OwnerResourceComponent->GetCurrentResourceType());
	}
}

void UFBCHUDWidget::BroadcastMaterialTypeChanged(EFBCResourceType NewMaterialType)
{
	OnResourceTypeChanged.Broadcast(NewMaterialType);
}
