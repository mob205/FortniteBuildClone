#pragma once

#include "Abilities/GameplayAbilityTargetTypes.h"
#include "Data/StructureMaterialTypes.h"
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
	EFBCMaterialType MaterialType{};

	// Location of structure
	UPROPERTY()
	FVector Location{};

	// Orientation of structure
	// Could also just be a yaw, since buildings are only free to rotate along Z-axis
	UPROPERTY()
	FRotator Rotation{};

	FBuildTargetData() {}
	FBuildTargetData(const FGameplayTag& StructureTag, EFBCMaterialType MaterialType, const FVector& Location, const FRotator& Rotation)
		: StructureTag(StructureTag), MaterialType(MaterialType), Location(Location), Rotation(Rotation)
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
	
		bOutSuccess = true;
		return true;
	}

	bool operator==(const FBuildTargetData& Other) const
	{
		return Location == Other.Location && Rotation == Other.Rotation
			&& StructureTag == Other.StructureTag && MaterialType == Other.MaterialType;
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