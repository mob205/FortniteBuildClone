// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/BuildResourceComponent.h"


UBuildResourceComponent::UBuildResourceComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UBuildResourceComponent::SwitchResource()
{
	int8 CurrentResourceIntegral = static_cast<int8>(CurrentResourceType);
	int8 NextResourceIntegral = (CurrentResourceIntegral + 1) % static_cast<int8>(EFBCResourceType::FBCMat_Max);
	CurrentResourceType = static_cast<EFBCResourceType>(NextResourceIntegral);

	OnResourceTypeChanged.Broadcast(CurrentResourceType);
}
