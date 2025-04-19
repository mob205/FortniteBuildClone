// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/FBCGameplayAbility.h"

#include "EnhancedInputSubsystems.h"

void UFBCGameplayAbility::AddAbilityInputMappingContext()
{
	if (!CurrentActorInfo->IsLocallyControlled()) { return; }
	
	APawn* AsPawn = Cast<APawn>(GetAvatarActorFromActorInfo());
	check(AsPawn);
	if (APlayerController* PC = Cast<APlayerController>(AsPawn->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = PC->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			AvatarInputSubsystem = Subsystem;
			Subsystem->AddMappingContext(AbilityMappingContext, 1);
		}
	}
}

void UFBCGameplayAbility::RemoveAbilityInputMappingContext()
{
	if (!CurrentActorInfo->IsLocallyControlled()) { return; }

	if (AvatarInputSubsystem)
	{
		AvatarInputSubsystem->RemoveMappingContext(AbilityMappingContext);
	}
}
