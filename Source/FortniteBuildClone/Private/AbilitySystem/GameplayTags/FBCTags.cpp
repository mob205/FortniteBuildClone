#include "AbilitySystem/GameplayTags/FBCTags.h"

#include "NativeGameplayTags.h"

namespace FBCTags
{
	// Movement
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sprinting, "Movement.Sprint.Sprinting", "Player is actively sprinting");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SprintingBlocked, "Movement.Sprint.SprintingBlocked", "Player is unable to sprint");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Crouching, "Movement.Crouching", "Player is crouching");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Airborne, "Movement.Airborne", "Player is in the air");

	// Item
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(MaxResourceRecoverable, "Item.Consumable.MaxResourceRecoverable", "The maximum amount of health or shields recoverable by this item");

	// Abilities
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(FireWeapon, "Abilities.Weapon.Fire", "Firing the currently equipped weapon");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityDamage, "Abilities.Damage", "Damage dealt by an ability");

	// Input
	UE_DEFINE_GAMEPLAY_TAG(InputFireDown, "Input.Fire.Down");
	UE_DEFINE_GAMEPLAY_TAG(InputFireReleased, "Input.Fire.Released");

}
