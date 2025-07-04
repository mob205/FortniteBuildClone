// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/APIData.h"

FString UAPIData::GetAPIEndpoint(const FGameplayTag& EndpointTag)
{
	const FString ResourceName = Resources.FindChecked(EndpointTag);

	return InvokeURL + "/" + Stage +"/" + ResourceName;
}
