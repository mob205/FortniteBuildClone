#include "AbilitySystem/GameplayTags/FBCTags.h"

#include "NativeGameplayTags.h"

namespace FBCTags
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sprinting, "Movement.Sprint.Sprinting", "Player is actively sprinting");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SprintingBlocked, "Movement.Sprint.SprintingBlocked", "Player is unable to sprint");
}
