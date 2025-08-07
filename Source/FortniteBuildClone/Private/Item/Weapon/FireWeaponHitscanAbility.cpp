// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/FireWeaponHitscanAbility.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/GameplayTags/FBCTags.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "FortniteBuildClone/FortniteBuildClone.h"
#include "Item/Weapon/WeaponBase.h"
#include "Player/FBCCharacterBase.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Component/LagCompensationComponent.h"
#include "GameFramework/GameStateBase.h"
#include "Interface/Damageable.h"
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

bool UFireWeaponHitscanAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	bool Result = (Weapon && Weapon->CanShoot()) && (!IsLocallyControlled() || (IsLocallyControlled() && Weapon->GetCurrentFireDelay() <= 0));
	return Result && Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
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


	FGameplayCueParameters CueParams;
	CueParams.SourceObject = Weapon;
	CueParams.TargetAttachComponent = Weapon->GetStaticMesh();
	ASC->ExecuteGameplayCue(GameplayCueTag, CueParams);

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
			Weapon->ResetFireDelay();
			Weapon->ModifyAmmo(-1);

			// Wait until next tick to give GAS a chance to send the target data RPC
			GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &ThisClass::EndAbilityLocally));
		}
	}
}

void UFireWeaponHitscanAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (IsValid(Weapon))
	{
		Weapon->OnFireDelayComplete.RemoveAll(this);
	}
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

void UFireWeaponHitscanAbility::TryStoredData()
{
	OnValidData(StoredDataHandle, {});
	StoredDataHandle.Clear();
}

// Reconstruct the client shot on the server
void UFireWeaponHitscanAbility::OnValidData(const FGameplayAbilityTargetDataHandle& Data, FGameplayTag GameplayTag)
{
	if (Weapon->GetCurrentFireDelay() > 0)
	{
		if (Weapon->GetCurrentFireDelay() <= ServerEarlyFireThreshold)
		{
			// The request is close enough to the actual fire delay
			// It was probably predicted to be valid, but ping variance resulted in the request coming faster than the last shot
			StoredDataHandle = Data;
			Weapon->OnFireDelayComplete.AddUObject(this, &ThisClass::TryStoredData);
		}
		
		return;
	}
	Weapon->ResetFireDelay();
	
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
		if (IDamageable* Damageable = Cast<IDamageable>(Hit.GetActor()))
		{
			FGameplayEffectContextHandle Context = FGameplayEffectContextHandle(UAbilitySystemGlobals::Get().AllocGameplayEffectContext());
			Context.AddInstigator(FBCOwner, Weapon);

			FGameplayEffectSpecHandle Spec = MakeOutgoingGameplayEffectSpec(OnHitEffectClass, 1);
			Spec.Data->SetContext(Context);
			Spec.Data->SetSetByCallerMagnitude(FBCTags::AbilityDamage, Weapon->GetWeaponItemData()->GetDamage());

			Damageable->Damage(Spec);
		}
	}

	Weapon->ModifyAmmo(-1);
	Weapon->HandleFireSpreadIncrease();
}
