// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/FBCCharacter.h"
#include "FBCCharacterMovementComponent.generated.h"

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_None		UMETA(Hidden),
	CMOVE_Slide		UMETA(DisplayName = "Slide"),
	CMOVE_Max		UMETA(Hidden),
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FORTNITEBUILDCLONE_API UFBCCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	class FSavedMove_FBC : public FSavedMove_Character
	{
		typedef FSavedMove_Character Super;

		uint8 bPrevWantsToCrouch:1;

	public:
		virtual void Clear() override;
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData) override;
		virtual void PrepMoveFor(ACharacter* C) override;
	};

	class FNetworkPredictionData_Client_FBC : public FNetworkPredictionData_Client_Character
	{
	public:
		FNetworkPredictionData_Client_FBC(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Character Super;

		virtual FSavedMovePtr AllocateNewMove() override;
	};

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

	UPROPERTY(Transient)
	AFBCCharacter* FBCCharacterOwner;

	bool bPrevWantsToCrouch;
	
public:
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	UFUNCTION(BlueprintPure)
	bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;

	virtual bool IsMovingOnGround() const override;
    virtual bool CanCrouchInCurrentState() const override;
	
protected:
	virtual void InitializeComponent() override;
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;
private:
	void EnterSlide();
	void ExitSlide();
	void PhysSlide(float DeltaTime, int32 Iterations);
	bool GetSlideSurface(FHitResult& Hit) const;
};
