// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/InteractAbility.h"

#include "FortniteBuildClone/FortniteBuildClone.h"
#include "Interface/Interactable.h"

void UInteractAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                       const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	APawn* AvatarPawn = Cast<APawn>(GetAvatarActorFromActorInfo());

	// Invalid avatar actor
	if (!AvatarPawn)
	{
		UE_LOG(LogFBC, Warning, TEXT("InteractedAbility: Attempted to interact with invalid avatar!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, false, true);
		return;
	}

	// Cache controller for later line tracing
	AvatarController = AvatarPawn->GetController();

	// "Tick" to continue polling for interactables
	GetWorld()->GetTimerManager().SetTimer(
		TickHandle,
		FTimerDelegate::CreateUObject(this, &UInteractAbility::PollInteractable),
		InteractPollDelay,
		true);

	PollInteractable();
}

void UInteractAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (IsValid(InteractActor))
	{
		IInteractable::Execute_StopInteract(InteractActor, GetAvatarActorFromActorInfo());
	}
	InteractActor = nullptr;
	
	GetWorld()->GetTimerManager().ClearTimer(TickHandle);
}

void UInteractAbility::PollInteractable()
{
	FVector StartLocation{};
	FRotator Rotation{};
	
	AvatarController->GetPlayerViewPoint(StartLocation, Rotation);

	FVector EndLocation = Rotation.Vector() * Range + StartLocation;

	FHitResult Hit{};
	if (!GetWorld()->LineTraceSingleByChannel(Hit, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility))
	{
		// Nothing was hit by cast
		EndAbility(GetCurrentAbilitySpecHandle(), CurrentActorInfo, GetCurrentActivationInfo(), false, true);
		return;
	}

	AActor* CurrentInteractActor = Hit.GetActor();
	
	// First encountered interact actor
	if (InteractActor == nullptr)
	{
		InteractActor = CurrentInteractActor;
		IInteractable::Execute_StartInteract(InteractActor, GetAvatarActorFromActorInfo());
	}
	// Not the original interact actor
	else if (InteractActor != Hit.GetActor())
	{
		EndAbility(GetCurrentAbilitySpecHandle(), CurrentActorInfo, GetCurrentActivationInfo(), false, true);
		return;
	}
	
}
