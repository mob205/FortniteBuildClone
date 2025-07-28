#pragma once
#include "NativeGameplayTags.h"

namespace FBCTags
{
	// Movement
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Sprinting);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SprintingBlocked);

	// Items
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(MaxResourceRecoverable);

	// Abilities
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(FireWeapon);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(AbilityDamage);

	// Input
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputFireDown);
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputFireReleased);

}
