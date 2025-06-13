// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/StructureMaterialTypes.h"
#include "UObject/Interface.h"
#include "MaterialSwitchable.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaterialTypeChangedMulticastSignature, EFBCMaterialType, NewType);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnMaterialTypeChangedSignature, EFBCMaterialType, NewType);

// This class does not need to be modified.
UINTERFACE()
class UMaterialSwitchable : public UInterface
{
	GENERATED_BODY()
};

class FORTNITEBUILDCLONE_API IMaterialSwitchable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SwitchMaterials();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	EFBCMaterialType GetCurrentMaterial();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void BindOnMaterialTypeChanged(const FOnMaterialTypeChangedSignature& Event);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UnbindOnMaterialTypeChanged(const FOnMaterialTypeChangedSignature& Event);
};
