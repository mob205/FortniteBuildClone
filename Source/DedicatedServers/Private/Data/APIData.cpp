// Fill out your copyright notice in the Description page of Project Settings.


#include "Data/APIData.h"

UAPIData::UAPIData()
{
	Verbs.Add(EAPIVerb::EAPIVerb_GET, "GET");
	Verbs.Add(EAPIVerb::EAPIVerb_POST, "POST");
}

FString UAPIData::GetAPIEndpoint(const FGameplayTag& EndpointTag) const
{
	const FAPIEndpointDescription Resource = Resources.FindChecked(EndpointTag);

	return InvokeURL + "/" + Stage + "/" + Resource.Endpoint;
}

FString UAPIData::GetVerb(const FGameplayTag& EndpointTag) const
{
	const FAPIEndpointDescription Resource = Resources.FindChecked(EndpointTag);

	return Verbs.FindChecked(Resource.Verb);
}
