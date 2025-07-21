// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameFramework/Actor.h"
#include "StructUtils/InstancedStruct.h"
#include "EquippedItemActor.generated.h"

class UGameplayAbility;
class AFBCCharacter;

DECLARE_DELEGATE(FOnItemInformationChangedSignature);

UCLASS()
class FORTNITEBUILDCLONE_API AEquippedItemActor : public AActor
{
	GENERATED_BODY()

public:
	AEquippedItemActor();

	FOnItemInformationChangedSignature OnItemInformationChanged;
	
	virtual void OnItemEquipped(AFBCCharacter* AvatarActor);
	virtual void OnItemUnequipped(AFBCCharacter* AvatarActor);

	UFUNCTION(BlueprintCallable)
	FInstancedStruct& GetItemInfo() const
	{
		return *ItemInformation;
	}

	UFUNCTION(BlueprintCallable)
	void MarkItemInfoDirty() const
	{
		OnItemInformationChanged.ExecuteIfBound();
	}
	
	void SetItemInfo(FInstancedStruct* ItemInfo) { ItemInformation = ItemInfo; }

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly)
	FName SocketName{};

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAnimMontage> EquipMontage{};

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UGameplayAbility>> GrantedAbilities{};

	UPROPERTY(Transient)
	TSet<FGameplayAbilitySpecHandle> GrantedAbilityHandles{};

	FInstancedStruct* ItemInformation{};
};

