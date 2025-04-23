#pragma once

#include "Abilities/GameplayAbilityTargetTypes.h"
#include "EditTargetData.generated.h"

USTRUCT()
struct FEditTargetData : public FGameplayAbilityTargetData
{
	GENERATED_BODY()

	UPROPERTY()
	int32 EditBitfield{};

	FEditTargetData() {}
	FEditTargetData(int32 InEditData) : EditBitfield(InEditData) {}

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
	
		bOutSuccess = true;
		return true;
	}

	bool operator==(const FEditTargetData& Other) const
	{
		return EditBitfield == Other.EditBitfield;
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