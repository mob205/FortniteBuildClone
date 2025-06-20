#pragma once

#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Data/StructureResourceTypes.h"
#include "BuildTargetData.generated.h"

USTRUCT()
struct FBuildTargetData : public FGameplayAbilityTargetData
{
	GENERATED_BODY()

	// Type of structure
	UPROPERTY()
	FGameplayTag StructureTag{};

	// Material of structure
	UPROPERTY()
	EFBCResourceType MaterialType{};

	// Location of structure
	UPROPERTY()
	FVector Location{};

	// Orientation of structure
	// Could also just be a yaw, since buildings are only free to rotate along Z-axis
	UPROPERTY()
	FRotator Rotation{};

	// Edit to use for the structure
	UPROPERTY()
	int32 Edit{};

	FBuildTargetData() {}
	FBuildTargetData(const FGameplayTag& StructureTag, EFBCResourceType MaterialType, const FVector& Location, const FRotator& Rotation, int32 Edit)
		: StructureTag(StructureTag), MaterialType(MaterialType), Location(Location), Rotation(Rotation), Edit(Edit)
	{}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}
	
	virtual FString ToString() const override
	{
		return TEXT("FBuildTargetData");
	}

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << StructureTag;
		Ar << MaterialType;
		Ar << Location;
		Ar << Rotation;
		Ar << Edit;
	
		bOutSuccess = true;
		return true;
	}

	bool operator==(const FBuildTargetData& Other) const
	{
		return Location == Other.Location && Rotation == Other.Rotation
			&& StructureTag == Other.StructureTag && MaterialType == Other.MaterialType
			&& Edit == Other.Edit;
	}
};

template<>
struct TStructOpsTypeTraits<FBuildTargetData> : public TStructOpsTypeTraitsBase2<FBuildTargetData>
{
	enum
	{
		WithNetSerializer = true,
		Identical = true
	};
};