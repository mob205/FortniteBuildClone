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
	if (Avatar)
	{
		OwnerInventoryComponent = Avatar->GetComponentByClass<UInventoryComponent>();
	}
	
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

	TArray<FGameplayAttribute> Attributes{};
	ASC->GetAllAttributes(Attributes);
	for (const auto& Attribute : Attributes)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(Attribute)
			.AddUObject(this, &UFBCHUDWidget::OnAttributeChange);
	}
	
	if (OwnerResourceComponent)
	{
		OwnerResourceComponent->OnResourceTypeChanged.AddDynamic(this, &UFBCHUDWidget::BroadcastMaterialTypeChanged);
	}
}

void UFBCHUDWidget::AssignOnAttributeChanged(FGameplayAttribute Attribute, FOnAttributeChangedSignature Callback)
{
	if (!OnAttributeChangedMap.Contains(Attribute))
	{
		OnAttributeChangedMap.Add(Attribute);
	}
	OnAttributeChangedMap[Attribute].Add(Callback);
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

	TArray<FGameplayAttribute> Attributes{};
	AbilitySystemComponent->GetAllAttributes(Attributes);
	for (const auto& Attribute : Attributes)
	{
		if(FOnAttributeChangedMulticastSignature* Delegate = OnAttributeChangedMap.Find(Attribute))
		{
			float AttributeValue = AbilitySystemComponent->GetNumericAttribute(Attribute);
			Delegate->Broadcast(AttributeValue);
		}
	}
}

void UFBCHUDWidget::OnAttributeChange(const FOnAttributeChangeData& OnAttributeChangeData)
{
	if (OnAttributeChangedMap.Contains(OnAttributeChangeData.Attribute))
	{
		OnAttributeChangedMap[OnAttributeChangeData.Attribute].Broadcast(OnAttributeChangeData.NewValue);
	}
}

void UFBCHUDWidget::BroadcastMaterialTypeChanged(EFBCResourceType NewMaterialType)
{
	OnResourceTypeChanged.Broadcast(NewMaterialType);
}
