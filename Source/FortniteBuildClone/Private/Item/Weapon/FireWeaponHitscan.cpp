// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Weapon/FireWeaponHitscan.h"

#include "AbilitySystemComponent.h"
#include "FBCBlueprintLibrary.h"
#include "AbilitySystem/GameplayTags/FBCTags.h"
#include "FortniteBuildClone/FortniteBuildClone.h"
#include "Item/Weapon/WeaponBase.h"
#include "Player/FBCCharacter.h"
#include "Player/FBCPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "KismetTraceUtils.h"
#include "Abilities/Tasks/AbilityTask_WaitTargetData.h"

UFireWeaponHitscan::UFireWeaponHitscan()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	SetAssetTags(FBCTags::FireWeapon.GetTag().GetSingleTagContainer());
}

void UFireWeaponHitscan::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	Weapon = Cast<AWeaponBase>(GetCurrentSourceObject());

	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("Pew?")));
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

	UAbilityTask_WaitTargetData* WaitTargetData = UAbilityTask_WaitTargetData::WaitTargetDataUsingActor(this, {}, EGameplayTargetingConfirmation::Custom, nullptr);
	WaitTargetData->ValidData.AddDynamic(this, &ThisClass::OnValidData);
	WaitTargetData->Activate();

	
	ASC->CallServerSetReplicatedTargetData()
	//ASC->ServerSetReplicatedTargetData()

	// Right now, abilities are only ever casted by FBCCharacter, so this shouldn't be null. Can be generalized later
	AFBCCharacter* FBCOwner = Cast<AFBCCharacter>(GetAvatarActorFromActorInfo());
	APlayerController* PC = FBCOwner->GetPlayerController();

	if (Weapon->GetStaticMesh()->DoesSocketExist("Muzzle"))
	{
		UE_LOG(LogTemp, Warning, TEXT("Found socket on %d"), HasAuthority(&ActivationInfo));
	}
	FVector Start = Weapon->GetStaticMesh()->GetSocketLocation("Muzzle");
	FVector End;
	
	FHitResult Hit;
	if (UFBCBlueprintLibrary::TraceControllerLook(PC, Range, Hit, ECC_Pawn))
	{
		End = Hit.Location;
	}
	else
	{
		FVector Location;
		FRotator Rotation;
		PC->GetPlayerViewPoint(Location, Rotation);
		End = Location + Rotation.Vector() * Range;
	}

	FGameplayCueParameters Params;
	Params.SourceObject = Weapon;
	Params.Location = End;
	ASC->ExecuteGameplayCue(GameplayCueTag, Params);
	
	DrawDebugLine(GetWorld(), Start, End, FColor::Red, true, 5);
	if (HasAuthority(&CurrentActivationInfo) && GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Pawn, FBCOwner->GetIgnoreCharacterParams()))
	{
		if (UAbilitySystemComponent* HitASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Hit.GetActor()))
		{
			FGameplayEffectContextHandle ContextHandle = HitASC->MakeEffectContext();
			FGameplayEffectSpecHandle SpecHandle = HitASC->MakeOutgoingSpec(OnHitEffectClass, 1, ContextHandle);
			HitASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red, FString::Printf(TEXT("Hitting %s"), *Hit.GetActor()->GetName()));
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

void UFireWeaponHitscan::OnValidData(const FGameplayAbilityTargetDataHandle& Data)
{
	UE_LOG(LogTemp, Warning, TEXT("Hello from %s"), HasAuthority(&CurrentActivationInfo) ? TEXT("Server") : TEXT("Client"));
}
