// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FBCBlueprintLibrary.h"
#include "AbilitySystem/Abilities/FBCGameplayAbility.h"
#include "Abilities/GameplayAbility.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "AbilitySystem/Abilities/Build/StructureTargetingActor.h"
#include "Data/StructureInfoDataAsset.h"
#include "EditAbility.generated.h"

class APlacedStructure;
class AEditTargetingActor;

/**
 * 
 */
UCLASS()
class FORTNITEBUILDCLONE_API UEditAbility : public UFBCGameplayAbility
{
	GENERATED_BODY()

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	AEditTargetingActor* InitializeFromStructureInfo(
		const FTransform& TargetTransform, const FGameplayTag& StructureTag);

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// Abilities with this tag are cancelled when this ability is executed and a structure is successfully selected
	UPROPERTY(EditDefaultsOnly, Category = "Tags", meta=(DisplayName="Cancel Abilities with Tag After Selected Structure"))
	FGameplayTagContainer SuccessfulActivationCancelTags{};

	UPROPERTY(EditDefaultsOnly, Category = "Editing")
	TObjectPtr<UStructureInfoDataAsset> StructureInfo{};

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float Range{};

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	FGameplayTag StartSelectionTag{};

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	FGameplayTag EndSelectionTag{};

	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	FGameplayTag ResetEditTag{};

private:
	template<typename T>
	T* GetSelection() const;
	
	
	TObjectPtr<APlacedStructure> SelectedStructure;
	TObjectPtr<AStructureTargetingActor> SelectedBuildTargetingActor{};

	const FEditMap* CurrentEditMap{};
	
	AEditTargetingActor* SpawnTargetingActor(const FTransform& SpawnTransform, TSubclassOf<AEditTargetingActor> ActorClass) const;
	TObjectPtr<AEditTargetingActor> TargetingActor;
	
	void EditStructure(int32 EditBitfield, int Yaw) const;

	UFUNCTION()
	void OnStructureEditDataReceived(const FGameplayAbilityTargetDataHandle& Data);

	UFUNCTION()
	void OnBuildTargetingEditDataReceived(const FGameplayAbilityTargetDataHandle& Data);
	
	UFUNCTION()
	void StartSelection(FGameplayEventData Payload);

	UFUNCTION()
	void EndSelection(FGameplayEventData Payload);
	
	UFUNCTION()
	void OnSelectedStructureDestroyed(AActor* DestroyedActor);
	
	UFUNCTION()
	void OnEditReset(FGameplayEventData Payload);

	bool bAllowRotations{};
	
	void ListenForInput();
	void HandleStructureEdit();
	void HandleBuildTargetingActorEdit();
};

template<typename T>
T* UEditAbility::GetSelection() const
{
	APawn* AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	
	check(AvatarPawn); // Currently, avatars are always pawns

	FHitResult HitResult{};
	if (!UFBCBlueprintLibrary::TraceControllerLook(AvatarPawn->GetController(), Range, HitResult))
	{
		return nullptr;
	}
	if (T* AsType = Cast<T>(HitResult.GetActor()))
	{
		return AsType;
	}
	return nullptr;
}