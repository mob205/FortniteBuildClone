// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/BuildResourceComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/FBCAttributeSet.h"


UBuildResourceComponent::UBuildResourceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBuildResourceComponent::SwitchResource()
{
	SetResourceType(GetNextResourceType(CurrentResourceType));
}

void UBuildResourceComponent::SetResourceType(EFBCResourceType NewResourceType)
{
	CurrentResourceType = NewResourceType;

	OnResourceTypeChanged.Broadcast(NewResourceType);
}

void UBuildResourceComponent::OnResourceCountChanged(EFBCResourceType ResourceType, float NewValue)
{
	if (ResourceType == CurrentResourceType && NewValue == 0)
	{
		// We ran out of this resource - try to find the next valid one

		EFBCResourceType OtherResourceType = CurrentResourceType;
		for (int i = 0; i < static_cast<int>(EFBCResourceType::FBCMat_Max) - 1; ++i)
		{
			OtherResourceType = GetNextResourceType(OtherResourceType);
			if (OwnerASC->GetNumericAttribute(UFBCAttributeSet::ResourceToAttributeMap[OtherResourceType]) != 0)
			{
				// Found a usable resource!
				SetResourceType(OtherResourceType);
				return;
			}
		}
		// No other resources were found - everything's empty
	}
}

void UBuildResourceComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerASC = GetOwner()->GetComponentByClass<UAbilitySystemComponent>();

	if (!OwnerASC) { return; }

	// Bind to resource count changes for auto-switching when hitting 0
	for (const auto& Resource : UFBCAttributeSet::ResourceToAttributeMap)
	{
		OwnerASC->GetGameplayAttributeValueChangeDelegate(Resource.Value)
			.AddLambda(
				[this, Resource](const FOnAttributeChangeData& Data)
				{
					OnResourceCountChanged(Resource.Key, Data.NewValue);
				}
			);
	}
}

EFBCResourceType UBuildResourceComponent::GetNextResourceType(EFBCResourceType ResourceType) const
{
	int8 CurrentResourceIntegral = static_cast<int8>(ResourceType);
	int8 NextResourceIntegral = (CurrentResourceIntegral + 1) % static_cast<int8>(EFBCResourceType::FBCMat_Max);
	return static_cast<EFBCResourceType>(NextResourceIntegral);
}
