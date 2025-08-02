#pragma once

#include "Abilities/GameplayAbilityTargetTypes.h"
#include "WeaponTargetData.generated.h"

USTRUCT()
struct FWeaponTargetData : public FGameplayAbilityTargetData
{
	GENERATED_BODY()

	UPROPERTY()
	double Timestamp{};

	UPROPERTY()
	FVector ViewLocation{};

	UPROPERTY()
	FRotator ViewRotation{};

	UPROPERTY()
	TArray<AFBCCharacter*> RelevantTargets{};

	FWeaponTargetData() {}
	FWeaponTargetData(double InTimestamp, const FVector& InViewLocation, const FRotator& InViewRotation)
		: Timestamp(InTimestamp), ViewLocation(InViewLocation), ViewRotation(InViewRotation)
	{}
	FWeaponTargetData(double InTimestamp, const FVector& InViewLocation, const FRotator& InViewRotation, const TArray<AFBCCharacter*>& InTargets)
		: Timestamp(InTimestamp), ViewLocation(InViewLocation), ViewRotation(InViewRotation), RelevantTargets(InTargets)
	{}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}
	
	virtual FString ToString() const override
	{
		return TEXT("FWeaponTargetData");
	}

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << Timestamp;
		Ar << ViewLocation;
		Ar << ViewRotation;
		Ar << RelevantTargets;
	
		bOutSuccess = true;
		return true;
	}
};

template<>
struct TStructOpsTypeTraits<FWeaponTargetData> : public TStructOpsTypeTraitsBase2<FWeaponTargetData>
{
	enum
	{
		WithNetSerializer = true,
	};
};