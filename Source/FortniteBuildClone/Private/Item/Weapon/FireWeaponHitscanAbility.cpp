// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/FireWeaponHitscanAbility.h"

#include "AbilitySystemComponent.h"
#include "FBCBlueprintLibrary.h"
#include "AbilitySystem/GameplayTags/FBCTags.h"
#include "FortniteBuildClone/FortniteBuildClone.h"
#include "Item/Weapon/WeaponBase.h"
#include "Player/FBCCharacter.h"
#include "Player/FBCPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/GameStateBase.h"
#include "Item/Weapon/WeaponTargetData.h"

UFireWeaponHitscanAbility::UFireWeaponHitscanAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	SetAssetTags(FBCTags::FireWeapon.GetTag().GetSingleTagContainer());
}

void UFireWeaponHitscanAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	FBCOwner = Cast<AFBCCharacter>(ActorInfo->AvatarActor);
	GameState = GetWorld()->GetGameState();
	
}

void UFireWeaponHitscanAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	Weapon = Cast<AWeaponBase>(GetCurrentSourceObject());
	
	if (!Weapon)
	{
		UE_LOG(LogFBC, Error, TEXT("FireWeaponHitscan: No weapon source object"));
		CancelAbility(Handle, ActorInfo, ActivationInfo, false);
		return;
	}

	if (Weapon->GetCurrentFireDelay() > 0)
	{
		UE_LOG(LogFBC, Error, TEXT("FireWeaponHitscan: Weapon has active fire delay"));
		CancelAbility(Handle, ActorInfo, ActivationInfo, false);
		return;
	}

	Weapon->ResetFireDelay();
	
	if (HasAuthority(&ActivationInfo))
	{
		ASC->AbilityTargetDataSetDelegate(CurrentSpecHandle, ActivationInfo.GetActivationPredictionKey()).AddUObject(this, &UFireWeaponHitscanAbility::OnValidData);
	}
	else
	{
		ASC->ServerSetReplicatedTargetData(CurrentSpecHandle, ActivationInfo.GetActivationPredictionKey(), GetAimingTargetData(), {}, ActivationInfo.GetActivationPredictionKey());

		// TODO: Predictions here using random stream
		FVector Res = Weapon->GetSpreadStream().VRandCone({}, 0);

		Weapon->HandleFireSpreadIncrease();

	}

	// Wait until next tick to give GAS a chance to send the target data RPC
	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ThisClass::EndAbilityLocally));
}

void UFireWeaponHitscanAbility::EndAbilityLocally()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

FGameplayAbilityTargetDataHandle UFireWeaponHitscanAbility::GetAimingTargetData() const
{
	double Timestamp = GameState->GetServerWorldTimeSeconds();
	FVector ViewLocation{};
	FRotator ViewRotation{};
	FBCOwner->GetPlayerController()->GetPlayerViewPoint(ViewLocation, ViewRotation);
	
	return { new FWeaponTargetData{Timestamp, ViewLocation, ViewRotation} };
}


// Reconstruct the client shot on the server
void UFireWeaponHitscanAbility::OnValidData(const FGameplayAbilityTargetDataHandle& Data,
                                     FGameplayTag GameplayTag)
{
	if (const FGameplayAbilityTargetData* BaseTargetData = Data.Get(0))
	{
		if (BaseTargetData->GetScriptStruct() == FWeaponTargetData::StaticStruct())
		{
			const FWeaponTargetData* ShotData = static_cast<const FWeaponTargetData*>(BaseTargetData);
			ServerFire(*ShotData);
		}
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

void UFireWeaponHitscanAbility::ServerFire(const FWeaponTargetData& TargetData) const
{
	if (!OnHitEffectClass) { return; }

	// TODO: Add validation - the view location and view rotation should be close to server's

	FVector Start = TargetData.ViewLocation;
	
	float HalfAngle = FMath::DegreesToRadians(Weapon->GetCurrentWeaponSpread() * .5f);
	FVector Direction = Weapon->GetSpreadStream().VRandCone(TargetData.ViewRotation.Vector(), HalfAngle);

	FVector EndNoSpread = Start + Range * TargetData.ViewRotation.Vector();
	FVector End = Start + Range * Direction;

	DrawDebugLine(GetWorld(), Start, EndNoSpread, FColor::Green, false, 20.f);

	FHitResult Hit;
	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, FBCOwner->GetIgnoreCharacterParams()))
	{
		if (Hit.bBlockingHit)
		{
			DrawDebugSphere(GetWorld(), Hit.Location, 25.f, 8, FColor::Red, false, 20.0f);
		}
		if (!Hit.bBlockingHit)
		{
			return;
		}
		if (UAbilitySystemComponent* HitASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Hit.GetActor()))
		{
			FGameplayEffectContextHandle ContextHandle = HitASC->MakeEffectContext();
			FGameplayEffectSpecHandle SpecHandle = HitASC->MakeOutgoingSpec(OnHitEffectClass, 1, ContextHandle);
			SpecHandle.Data->SetSetByCallerMagnitude(FBCTags::AbilityDamage, Weapon->GetWeaponItemData()->GetDamage());
			HitASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data); }
	}

	Weapon->HandleFireSpreadIncrease();
}
