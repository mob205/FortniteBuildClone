// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "FBCCharacter.generated.h"

class UFBCAttributeSet;
class UFBCAbilitySystemComponent;
class AFBCPlayerController;
class UGameplayAbility;
class UGameplayEffect;
class UStructureInfoDataAsset;
class UInputAction;

UCLASS()
class FORTNITEBUILDCLONE_API AFBCCharacter : public ACharacter, public IAbilitySystemInterface
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

private:
	void InitAbilityActorInfo();

	UPROPERTY(EditDefaultsOnly, Category = "Ability System")
	TArray<TSubclassOf<UGameplayAbility>> InitialAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Ability System")
	TSubclassOf<UGameplayEffect> InitialAttributesEffect;
	
	void GrantInitialAbilities();
	void InitializeAttributes();

	UPROPERTY()
	TObjectPtr<UFBCAbilitySystemComponent> ASC;

	UPROPERTY()
	TObjectPtr<UFBCAttributeSet> AS;
	
	FGameplayTag BuildAbilityTag;

	UFUNCTION(Server, Reliable)
	void ServerOnBuildAction(FGameplayTag StructureTag);
	
	void HandleBuildAction(const FGameplayTag StructureTag) const;
};
