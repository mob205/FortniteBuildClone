// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/StructureResourceTypes.h"
#include "BuildResourceComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResourceTypeChangedSignature, EFBCResourceType, NewResourceType);

UCLASS(BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FORTNITEBUILDCLONE_API UBuildResourceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBuildResourceComponent();

	UPROPERTY(BlueprintAssignable)
	FOnResourceTypeChangedSignature OnResourceTypeChanged;
	
	// Switches to the next resource type
	UFUNCTION(BlueprintCallable)
	void SwitchResource();

	UFUNCTION(BlueprintCallable)
	EFBCResourceType GetCurrentResourceType() const { return CurrentResourceType; }
	
private:
	EFBCResourceType CurrentResourceType{ EFBCResourceType::FBCMat_Wood };
};
