// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "FBCCharacterBase.generated.h"

class UInventoryComponent;
class ULagCompensationComponent;
class UBoxComponent;
class UFBCAttributeSet;
class UFBCAbilitySystemComponent;
class UGameplayAbility;
class UGameplayEffect;
class UStructureInfoDataAsset;
class UInputAction;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnASCInitSignature, UAbilitySystemComponent*);

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

UCLASS(Abstract)
class FORTNITEBUILDCLONE_API AFBCCharacterBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AFBCCharacterBase();
	
	UPROPERTY(BlueprintReadOnly, Replicated, Category=Character)
	uint8 bIsSliding:1;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	ULagCompensationComponent* GetLagCompensationComponent() const { return LagCompensationComponent; }
	
	FCollisionQueryParams GetIgnoreCharacterParams() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Building")
	TObjectPtr<UStructureInfoDataAsset> StructureInfo;

	FOnASCInitSignature OnASCInit;
protected:
	virtual void PossessedBy(AController* NewController) override;
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void InitAbilityActorInfo();

	UFUNCTION(BlueprintCallable)
	void OnBuildAction(UInputAction* InputAction);
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UFBCAbilitySystemComponent> ASC;

	UPROPERTY()
	TObjectPtr<UFBCAttributeSet> AS;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<ULagCompensationComponent> LagCompensationComponent;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<UInventoryComponent> InventoryComponent;

private:
	void OnAbilityFailed(const UGameplayAbility* GameplayAbility, const FGameplayTagContainer& GameplayTags);
	
	UPROPERTY(EditDefaultsOnly, Category = "Ability System")
	TArray<FInitialAbility> InitialAbilities;

	UPROPERTY(EditDefaultsOnly, Category = "Ability System")
	TSubclassOf<UGameplayEffect> InitialAttributesEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Ability System")
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;
	
	void GrantInitialAbilities();
	void InitializeAttributes();
	void AddInitialEffects();
	
	void HandleBuildAction(const FGameplayTag StructureTag) const;

public:
	const TArray<UBoxComponent*>& GetHitboxes() const { return Hitboxes;}
private:
	void CacheHitboxes();
	TArray<UBoxComponent*> Hitboxes{};
};
