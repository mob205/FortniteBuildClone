#include "AbilitySystem/GameplayTags/FBCTags.h"

#include "NativeGameplayTags.h"

namespace FBCTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sprinting, "Movement.Sprint.Sprinting", "Player is actively sprinting");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SprintingBlocked, "Movement.Sprint.SprintingBlocked", "Player is unable to sprint");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(MaxResourceRecoverable, "Item.Consumable.MaxResourceRecoverable", "The maximum amount of health or shields recoverable by this item");
}
