// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "FBCCharacter.generated.h"

class AFBCPlayerState;
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
class FORTNITEBUILDCLONE_API AFBCCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Replicated, Category=Character)
	uint8 bIsSliding:1;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	FCollisionQueryParams GetIgnoreCharacterParams() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
	TObjectPtr<UStructureInfoDataAsset> StructureInfo;
	
protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void OnBuildAction(UInputAction* InputAction);

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AFBCPlayerController> PlayerController;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UFBCAbilitySystemComponent> ASC;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AFBCPlayerState> FBCPlayerState;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

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
