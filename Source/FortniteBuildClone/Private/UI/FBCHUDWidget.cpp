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
	for (const auto& Resource : UFBCAttributeSet::ResourceToAttributeMap)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(Resource.Value)
			.AddLambda(
				[this, Resource](const FOnAttributeChangeData& Data)
				{
					OnResourceCountChanged.Broadcast(Resource.Key, Data.NewValue);
				}
			);
	}

	if (OwnerResourceComponent)
	{
		OwnerResourceComponent->OnResourceTypeChanged.AddDynamic(this, &UFBCHUDWidget::BroadcastMaterialTypeChanged);
	}
}

void UFBCHUDWidget::BroadcastInitialValues()
{
	for (const auto& Resource : UFBCAttributeSet::ResourceToAttributeMap)
	{
		OnResourceCountChanged.Broadcast(Resource.Key, AbilitySystemComponent->GetNumericAttribute(Resource.Value));
	}

	if (OwnerResourceComponent)
	{
		OnResourceTypeChanged.Broadcast(OwnerResourceComponent->GetCurrentResourceType());
	}
}

void UFBCHUDWidget::BroadcastMaterialTypeChanged(EFBCResourceType NewMaterialType)
{
	OnResourceTypeChanged.Broadcast(NewMaterialType);
}
