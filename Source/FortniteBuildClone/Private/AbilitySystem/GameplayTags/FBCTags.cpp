#include "AbilitySystem/GameplayTags/FBCTags.h"

#include "NativeGameplayTags.h"

namespace FBCTags
{
	// Movement
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Sprinting, "Movement.Sprint.Sprinting", "Player is actively sprinting");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(SprintingBlocked, "Movement.Sprint.SprintingBlocked", "Player is unable to sprint");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(RunningBlocked, "Movement.Sprint.RunningBlocked", "Player is unable to run (can only walk)");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Crouching, "Movement.Crouching", "Player is crouching");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Airborne, "Movement.Airborne", "Player is in the air");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AimingDownSights, "Movement.ADS", "Player is actively aiming down sights with an equipped weapon");

	// Item
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(MaxResourceRecoverable, "Item.Consumable.MaxResourceRecoverable", "The maximum amount of health or shields recoverable by this item");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(EquippedItemBlocked, "Item.Inventory.Blocked", "The currently selected item cannot be equipped");
	
	// Abilities
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(FireWeapon, "Abilities.Weapon.Fire", "Firing the currently equipped weapon");
	UE_DEFINE_GAMEPLAY_TAG(ReloadWeapon, "Abilities.Weapon.Reload");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AbilityDamage, "Abilities.Damage", "Damage dealt by an ability");
	UE_DEFINE_GAMEPLAY_TAG(Build, "Abilities.Build")
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Harvesting, "Abilities.Harvesting", "Associated with harvesting resources from things in the world")

	// Input
	UE_DEFINE_GAMEPLAY_TAG(InputFireDown, "Input.Fire.Down");
	UE_DEFINE_GAMEPLAY_TAG(InputFireReleased, "Input.Fire.Released");

	UE_DEFINE_GAMEPLAY_TAG(AltFireDown, "Input.AltFire.Down");
	UE_DEFINE_GAMEPLAY_TAG(AltFireReleased, "Input.AltFire.Released");

}
