#pragma once

#include "CoreMinimal.h"
#include "BitGrid.generated.h"

USTRUCT(BlueprintType)
struct FBitGrid
{
	GENERATED_BODY()

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