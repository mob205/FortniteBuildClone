#pragma once

#include "ItemTypes.generated.h"

USTRUCT(BlueprintType)
struct FItemInfoConsumable
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	uint8 Count{1};
};
