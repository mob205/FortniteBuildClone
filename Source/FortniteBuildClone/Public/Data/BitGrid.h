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
};

FORCEINLINE uint32 GetTypeHash(const FBitGrid& BitGrid)
{
	return GetTypeHash(BitGrid.BitField);
}