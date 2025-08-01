// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/FBCCharacter.h"
#include "AbilitySystem/FBCAbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffect.h"
#include "Structure/Data/StructureInfoDataAsset.h"
#include "InputAction.h"
#include "AbilitySystem/GameplayTags/FBCTags.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "FortniteBuildClone/FortniteBuildClone.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"
#include "Player/FBCPlayerController.h"
#include "Player/FBCPlayerState.h"

AFBCCharacter::AFBCCharacter()
{
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArmComponent->SetupAttachment(GetRootComponent());

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("Camera");
	CameraComponent->SetupAttachment(SpringArmComponent);
}

UAbilitySystemComponent* AFBCCharacter::GetAbilitySystemComponent() const
{
	return ASC;
}

FCollisionQueryParams AFBCCharacter::GetIgnoreCharacterParams() const
{
	FCollisionQueryParams Params{};

	Params.AddIgnoredActor(this);

	return Params;
}

// Called on server only
void AFBCCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	InitAbilityActorInfo();
	GrantInitialAbilities();
	InitializeAttributes();
	AddInitialEffects();

	CacheHitboxes();
}

// Called on clients only
void AFBCCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitAbilityActorInfo();
}

void AFBCCharacter::BeginPlay()
{
	Super::BeginPlay();

	BuildAbilityTag = FGameplayTag::RequestGameplayTag("Abilities.Build");
}

void AFBCCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(AFBCCharacter, bIsSliding, COND_None, REPNOTIFY_Always);
}

void AFBCCharacter::InitAbilityActorInfo()
{
	FBCPlayerState = GetPlayerState<AFBCPlayerState>();
	check(FBCPlayerState);

	ASC = FBCPlayerState->GetAbilitySystemComponent();
	ASC->InitAbilityActorInfo(FBCPlayerState, this);
	
	AS = FBCPlayerState->GetAttributeSet();
	
	PlayerController = Cast<AFBCPlayerController>(GetController());
	
	OnASCInit.Broadcast(ASC);

	ASC->AbilityFailedCallbacks.AddUObject(this, &AFBCCharacter::OnAbilityFailed);
	ASC->RegisterGameplayTagEvent(FBCTags::AimingDownSights).AddUObject(this, &AFBCCharacter::HandleADS);
}

void AFBCCharacter::OnAbilityFailed(const UGameplayAbility* GameplayAbility, const FGameplayTagContainer& GameplayTags)
{
	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Red,
		FString::Printf(TEXT("Failed to activate ability %s"), *GameplayAbility->GetName()));
}

void AFBCCharacter::HandleADS(FGameplayTag GameplayTag, int Count)
{
#if !UE_SERVER
	SetCameraADS(Count != 0);
#endif
}

void AFBCCharacter::GrantInitialAbilities()
{
	for (const auto& Ability : InitialAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec{Ability.Ability, 1};
		ASC->GiveAbility(AbilitySpec);

		if (Ability.bActivateImmediately)
		{
			ASC->TryActivateAbility(AbilitySpec.Handle);
		}
	}
}

void AFBCCharacter::InitializeAttributes()
{
	if (!InitialAttributesEffect) { return; }

	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	FGameplayEffectSpecHandle Effect = ASC->MakeOutgoingSpec(InitialAttributesEffect, 1, Context);
	ASC->ApplyGameplayEffectSpecToSelf(*Effect.Data);
}

void AFBCCharacter::AddInitialEffects()
{
	for (const auto& Effect : InitialEffects)
	{
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(Effect, 1, Context);
		ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data);
	}
}

void AFBCCharacter::OnBuildAction(UInputAction* InputAction)
{
	// Get structure tag associated with the input
	FGameplayTag StructureTag = StructureInfo->GetTagFromInput(InputAction);

	if (!StructureTag.IsValid())
	{
		UE_LOG(LogFBC, Warning, TEXT("Invalid build action [%s] provided to OnBuildAction."), *InputAction->GetName());
	}

	// Alert the ability of switching structure locally
	HandleBuildAction(StructureTag);
}

void AFBCCharacter::HandleBuildAction(const FGameplayTag StructureTag) const
{
	ASC->TryActivateAbilitiesByTag(BuildAbilityTag.GetSingleTagContainer());
	FGameplayEventData Payload{};
	ASC->HandleGameplayEvent(StructureTag, &Payload);
}

void AFBCCharacter::CacheHitboxes()
{
	TArray<USceneComponent*> Children{};
	GetMesh()->GetChildrenComponents(false, Children);

	for (const auto Child : Children)
	{
		if (UBoxComponent* Hitbox = Cast<UBoxComponent>(Child))
		{
			Hitboxes.Add(Hitbox);
		}
	}
}
