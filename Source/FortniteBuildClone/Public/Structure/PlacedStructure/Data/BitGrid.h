#pragma once

#include "CoreMinimal.h"
#include "BitGrid.generated.h"

USTRUCT(BlueprintType)
struct FBitGrid
{
	GENERATED_BODY()

	FBitGrid() = default;
	
	FBitGrid(int32 InBitField)
		: BitField(InBitField)
	{}
	
	UPROPERTY(EditAnywhere)
	int Dimensions{3};

	UPROPERTY(EditAnywhere)
	int32 BitField{};

	bool operator==(const FBitGrid& Other) const
	{
		return BitField == Other.BitField;
	}
	bool operator==(int32 Other) const
	{
		return BitField == Other;
	}

	operator int32() const
	{
		return BitField;
	}

	bool Get(int Index) const
	{
		return BitField & (1 << Index);	
	}
	void Set(int Index)
	{
		BitField |= (1 << Index);
	}
	void Reset(int Index)
	{
		BitField &= ~(1 << Index);
	}
};

FORCEINLINE uint32 GetTypeHash(const FBitGrid& BitGrid)
{
	return GetTypeHash(BitGrid.BitField);
}