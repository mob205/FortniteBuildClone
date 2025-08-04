// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/FireWeaponHitscanAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/GameplayTags/FBCTags.h"
#include "FortniteBuildClone/FortniteBuildClone.h"
#include "Item/Weapon/WeaponBase.h"
#include "Player/FBCCharacterBase.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Component/LagCompensationComponent.h"
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

	FBCOwner = Cast<AFBCCharacterBase>(ActorInfo->AvatarActor);
	Weapon = Cast<AWeaponBase>(GetCurrentSourceObject());
	GameState = GetWorld()->GetGameState();
}

void UFireWeaponHitscanAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	
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

	if (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted)
	{
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
			Weapon->ModifyAmmo(-1);
		}
	}

	// Wait until next tick to give GAS a chance to send the target data RPC
	GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ThisClass::EndAbilityLocally));
}

bool UFireWeaponHitscanAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	bool Result = (Weapon && Weapon->CanShoot());
	return Result && Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
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
	FBCOwner->GetController()->GetPlayerViewPoint(ViewLocation, ViewRotation);

	TArray<AFBCCharacterBase*> RelevantTargets{};
	FHitResult Hit{};
	if (GetWorld()->LineTraceSingleByChannel(Hit, ViewLocation, ViewLocation + Range * ViewRotation.Vector(), ECC_Visibility, FBCOwner->GetIgnoreCharacterParams()))
	{
		if (AFBCCharacterBase* Target = Cast<AFBCCharacterBase>(Hit.GetActor()))
		{
			RelevantTargets.Add(Target);
		}
	}
	
	return { new FWeaponTargetData{Timestamp, ViewLocation, ViewRotation, RelevantTargets } };
}


// Reconstruct the client shot on the server
void UFireWeaponHitscanAbility::OnValidData(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag GameplayTag)
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

	FVector End = Start + Range * Direction;

	FVector EndNoSpread = Start + Range * TargetData.ViewRotation.Vector();

	FHitResult Hit;
	{
		FLagCompensatedWindow CompensationWindow{TargetData.RelevantTargets, TargetData.Timestamp };
		GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, FBCOwner->GetIgnoreCharacterParams());
	}

	if (Cast<AFBCCharacterBase>(Hit.GetActor()))
	{
		DrawDebugLine(GetWorld(), Start, EndNoSpread, FColor::Green, false, 20.f);
	}
	else
	{
		DrawDebugLine(GetWorld(), Start, EndNoSpread, FColor::Red, false, 20.f);
	}
	
	if (Hit.bBlockingHit)
	{
		if (UAbilitySystemComponent* HitASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Hit.GetActor()))
		{
			FGameplayEffectContextHandle ContextHandle = HitASC->MakeEffectContext();
			ContextHandle.AddInstigator(FBCOwner, Weapon);
			
			FGameplayEffectSpecHandle SpecHandle = HitASC->MakeOutgoingSpec(OnHitEffectClass, 1, ContextHandle);
			SpecHandle.Data->SetSetByCallerMagnitude(FBCTags::AbilityDamage, Weapon->GetWeaponItemData()->GetDamage());
			HitASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
		}
	}

	Weapon->ModifyAmmo(-1);
	Weapon->HandleFireSpreadIncrease();
}
