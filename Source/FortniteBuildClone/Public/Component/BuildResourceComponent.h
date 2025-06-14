// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/StructureMaterialTypes.h"
#include "BuildResourceComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnResourceTypeChangedSignature, EFBCMaterialType, NewResourceType);

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
	EFBCMaterialType GetCurrentResourceType() const { return CurrentResourceType; }
	
private:
	EFBCMaterialType CurrentResourceType{ EFBCMaterialType::FBCMat_Wood };
};
