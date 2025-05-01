#pragma once

#include "Abilities/GameplayAbilityTargetTypes.h"
#include "EditTargetData.generated.h"

USTRUCT()
struct FEditTargetData : public FGameplayAbilityTargetData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 EditBitfield{};

	// Yaw as number of 90-degree clockwise turns
	UPROPERTY()
	int8 YawCWTurns{};

	FEditTargetData() {}
	FEditTargetData(int32 InEditData, int8 InYaw) : EditBitfield(InEditData), YawCWTurns(InYaw) {}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}
	
	virtual FString ToString() const override
	{
		return TEXT("FEditTargetData");
	}

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << EditBitfield;
		Ar << YawCWTurns;
	
		bOutSuccess = true;
		return true;
	}

	bool operator==(const FEditTargetData& Other) const
	{
		return EditBitfield == Other.EditBitfield && YawCWTurns == Other.YawCWTurns;
	}
};

template<>
struct TStructOpsTypeTraits<FEditTargetData> : public TStructOpsTypeTraitsBase2<FEditTargetData>
{
	enum
	{
		WithNetSerializer = true,
		Identical = true
	};
};