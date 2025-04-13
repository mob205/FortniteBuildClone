// Fill out your copyright notice in the Description page of Project Settings.


#include "TraversableComponent.h"

UTraversableComponent::UTraversableComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

bool UTraversableComponent::IsValidLedge(USplineComponent* Ledge) const
{
	if (CheckIsValidLedge.IsBound())
	{
		return CheckIsValidLedge.Execute(Ledge);
	}
	return true;
}
