// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/FBCCharacter.h"
#include "FBCCharacterMovementComponent.generated.h"

class UCurveFloat;

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_None		UMETA(Hidden),
	CMOVE_Slide		UMETA(DisplayName = "Slide"),
	CMOVE_Max		UMETA(Hidden),
};

// Server to client about acknowledge move
struct FFBCMoveResponseDataContainer : FCharacterMoveResponseDataContainer
{
	using Super = FCharacterMoveResponseDataContainer;

	virtual void ServerFillResponseData(const UCharacterMovementComponent& CharacterMovement, const FClientAdjustment& PendingAdjustment) override;
	virtual bool Serialize(UCharacterMovementComponent& CharacterMovementComponent, FArchive& Ar, UPackageMap* PackageMap) override;

	float Stamina;
	float CurrentStaminaRegenDelay;
	bool bStaminaDrained;
};

// Client to server, encapsulates a move
struct FFBCNetworkMoveData : FCharacterNetworkMoveData
{
	using Super = FCharacterNetworkMoveData;

	FFBCNetworkMoveData()
		: Stamina{0}, CurrentStaminaRegenDelay{0}
	{}

	virtual void ClientFillNetworkMoveData(const FSavedMove_Character& ClientMove, ENetworkMoveType MoveType) override;
	virtual bool Serialize(UCharacterMovementComponent& CharacterMovement, FArchive& Ar, UPackageMap* PackageMap, ENetworkMoveType MoveType) override;
	
	float Stamina;
	float CurrentStaminaRegenDelay;
};

// Client to server. Used as network RPC parameter
struct FFBCNetworkMoveDataContainer : FCharacterNetworkMoveDataContainer
{
	using Super = FCharacterNetworkMoveDataContainer;

	FFBCNetworkMoveDataContainer()
	{
		NewMoveData = &MoveData[0];
		PendingMoveData = &MoveData[1];
		OldMoveData = &MoveData[2];
	}
private:
	FFBCNetworkMoveData MoveData[3];
};

class FSavedMove_FBC : public FSavedMove_Character
{
	using Super = FSavedMove_Character;
public:
	FSavedMove_FBC()
		: bWantsToSprint{}, bPrevWantsToCrouch{}, bStaminaDrained{}, StartStamina{}, EndStamina{}
	{}

	virtual ~FSavedMove_FBC() override {}
	
	uint8 bWantsToSprint:1;
	uint8 bPrevWantsToCrouch:1;
	uint8 bStaminaDrained:1;
	uint8 bPrevWasSprinting:1;

	float StartStamina;
	float EndStamina;
	float CurrentStaminaRegenDelay;

public:
	virtual void Clear() override;
	virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
	virtual void PrepMoveFor(ACharacter* C) override;
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
	virtual void CombineWith(const FSavedMove_Character* OldMove, ACharacter* InCharacter, APlayerController* PC, const FVector& OldStartLocation) override;
	virtual void SetInitialPosition(ACharacter* C) override;
	virtual void PostUpdate(ACharacter* C, EPostUpdateMode PostUpdateMode) override;
	virtual uint8 GetCompressedFlags() const override;
};

class FNetworkPredictionData_Client_FBC : public FNetworkPredictionData_Client_Character
{
public:
	FNetworkPredictionData_Client_FBC(const UCharacterMovementComponent& ClientMovement);

	typedef FNetworkPredictionData_Client_Character Super;

	virtual FSavedMovePtr AllocateNewMove() override;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FORTNITEBUILDCLONE_API UFBCCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UFBCCharacterMovementComponent();

	virtual void OnClientCorrectionReceived(class FNetworkPredictionData_Client_Character& ClientData, float TimeStamp, FVector NewLocation, FVector NewVelocity, UPrimitiveComponent* NewBase, FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition, uint8 ServerMovementMode, FVector ServerGravityDirection) override;
	virtual bool ServerCheckClientError(float ClientTimeStamp, float DeltaTime, const FVector& Accel, const FVector& ClientWorldLocation, const FVector& RelativeClientLocation, UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode) override;
	
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	UFUNCTION(BlueprintPure)
	bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;

	UFUNCTION(BlueprintCallable)
	void ToggleWantsToSprint(bool bNewWantsToSprint);
	
	UFUNCTION(BlueprintCallable)
	bool GetWantsToSprint() const { return bWantsToSprint; }

	UFUNCTION(BlueprintCallable)
	bool CanSprint() const;

	UFUNCTION(BlueprintCallable)
	bool IsSprinting() const { return bWantsToSprint && CanSprint();}

	virtual bool IsMovingOnGround() const override;
	virtual bool CanCrouchInCurrentState() const override;
	
	float GetStamina() const { return Stamina; }
	float GetMaxStamina() const { return MaxStamina; }
	bool IsStaminaDrained() const { return bStaminaDrained; }
	float GetCurrentStaminaRegenDelay() const { return CurrentStaminaRegenDelay; }

	void SetStamina(float NewStamina);
	void SetMaxStamina(float NewMaxStamina);
	void SetStaminaDrained(bool bNewValue);
	void SetCurrentStaminaRegenDelay(float RegenDelay);

	void OnStaminaDrained();
	void OnStaminaDrainRecovered();
	
	bool bWantsToSprint;
	bool bPrevWantsToCrouch;
	bool bPrevWasSprinting;
	float CurrentStaminaRegenDelay{};
protected:
	// Parameters
	UPROPERTY(EditDefaultsOnly, Category = "Character Movement: Sliding")
	float Slide_MinSpeed = 350;

	UPROPERTY(EditDefaultsOnly, Category = "Character Movement: Sliding")
	float Slide_EnterImpulse = 500;

	UPROPERTY(EditDefaultsOnly, Category = "Character Movement: Sliding")
	float Slide_GravityForce = 5000;
	
	UPROPERTY(EditDefaultsOnly, Category = "Character Movement: Sliding")
	float Slide_Friction = 1.3;

	UPROPERTY(EditDefaultsOnly, Category = "Character Movement: Walking")
	TObjectPtr<UCurveFloat> StrafeSpeedMapCurve{};

	UPROPERTY(EditDefaultsOnly, Category = "Character Movement: Walking")
	FVector WalkSpeeds{200, 180, 150};
	
	UPROPERTY(EditDefaultsOnly, Category = "Character Movement: Walking")
	FVector RunSpeeds{500, 350, 300};

	UPROPERTY(EditDefaultsOnly, Category = "Character Movement: Sprinting")
	FVector SprintSpeeds{700, 700, 700};

	UPROPERTY(EditDefaultsOnly, Category = "Character Movement: Walking")
	float WalkAcceleration{800};

	UPROPERTY(EditDefaultsOnly, Category = "Character Movement: Sprinting")
	FVector2D SprintAccelerationRange{800, 300};

	UPROPERTY(EditDefaultsOnly, Category = "Character Movement: Sprinting")
	float NetworkStaminaCorrectionThreshold;

	UPROPERTY(EditDefaultsOnly, Category = "Character Movement: Stamina")
	float StaminaDrainRate;

	UPROPERTY(EditDefaultsOnly, Category = "Character Movement: Stamina")
	float StaminaRegenRate;

	UPROPERTY(EditDefaultsOnly, Category = "Character Movement: Stamina")
	float StaminaRegenDelay{};
	
	virtual void OnStaminaChanged(float PrevValue, float NewValue);
	virtual void OnMaxStaminaChanged(float PrevValue, float NewValue);
public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetOwnerASC(UAbilitySystemComponent* NewOwnerASC);
private:
	FFBCMoveResponseDataContainer FBCMoveResponseDataContainer;
	FFBCNetworkMoveDataContainer FBCNetworkMoveDataContainer;
	
protected:
	UPROPERTY(Transient)
	TObjectPtr<AFBCCharacter> FBCCharacterOwner;

	UPROPERTY(Transient)
	TObjectPtr<UAbilitySystemComponent> OwnerASC;
	
	UPROPERTY()
	float Stamina;
	
	UPROPERTY(EditDefaultsOnly)
	float DefaultMaxStamina{};
	
	virtual void InitializeComponent() override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	virtual float GetMaxAcceleration() const override;
	virtual void SetBase(UPrimitiveComponent* NewBase, const FName BoneName = NAME_None, bool bNotifyActor = true) override;
	virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) override;
	virtual void PerformMovement(float DeltaTime) override;
private:
	void EnterSlide();
	void ExitSlide();
	void PhysSlide(float DeltaTime, int32 Iterations);
	bool GetSlideSurface(FHitResult& Hit) const;

	float GetWalkSpeed() const;
private:
	UPROPERTY()
	float MaxStamina;

	UPROPERTY()
	bool bStaminaDrained;

	void ToggleMovementTag(UAbilitySystemComponent* ASC, const FGameplayTag& Tag, bool Condition);
};
