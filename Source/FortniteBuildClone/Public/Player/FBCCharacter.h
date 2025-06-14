// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "Interface/MaterialSwitchable.h"
#include "FBCCharacter.generated.h"

class UFBCAttributeSet;
class UFBCAbilitySystemComponent;
class AFBCPlayerController;
class UGameplayAbility;
class UGameplayEffect;
class UStructureInfoDataAsset;
class UInputAction;


USTRUCT(BlueprintType)
struct FInitialAbility
{
	GENERATED_BODY()

	// The class of the gameplay ability to grant
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayAbility> Ability{};

	// If true, this ability is activated immediately after granting it
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bActivateImmediately{};
};

UCLASS()
class FORTNITEBUILDCLONE_API AFBCCharacter : public ACharacter, public IAbilitySystemInterface, public IMaterialSwitchable
{
	GENERATED_BODY()

public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
	TObjectPtr<UStructureInfoDataAsset> StructureInfo;
	
protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void OnBuildAction(UInputAction* InputAction);

	UPROPERTY(BlueprintReadOnly)
	AFBCPlayerController* PlayerController;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UFBCAbilitySystemComponent> ASC;

protected:
	// Currently this is local only. If needed, can send this information to server via RPC
	UPROPERTY(BlueprintReadOnly)
	EFBCMaterialType CurrentSelectedMaterial{};

	FOnMaterialTypeChangedMulticastSignature OnMaterialTypeChanged;
	
	virtual void SwitchMaterials_Implementation() override;
	virtual EFBCMaterialType GetCurrentMaterial_Implementation() override { return CurrentSelectedMaterial; }

	virtual void BindOnMaterialTypeChanged_Implementation(const FOnMaterialTypeChangedSignature& Event) override { OnMaterialTypeChanged.Add(Event); }
	virtual void UnbindOnMaterialTypeChanged_Implementation(const FOnMaterialTypeChangedSignature& Event) override { OnMaterialTypeChanged.Remove(Event); }

private:
	void InitAbilityActorInfo();

	UPROPERTY(EditDefaultsOnly, Category = "Ability System")
	TArray<FInitialAbility> InitialAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Ability System")
	TSubclassOf<UGameplayEffect> InitialAttributesEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Ability System")
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;
	
	void GrantInitialAbilities();
	void InitializeAttributes();
	void AddInitialEffects();

	UPROPERTY()
	TObjectPtr<UFBCAttributeSet> AS;
	
	FGameplayTag BuildAbilityTag;
	
	void HandleBuildAction(const FGameplayTag StructureTag) const;
	
};
