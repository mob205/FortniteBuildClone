// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/FBCCharacterMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "FortniteBuildClone/FortniteBuildClone.h"
#include "GameFramework/Character.h"

void UFBCCharacterMovementComponent::FSavedMove_FBC::Clear()
{
	FSavedMove_Character::Clear();

	bPrevWantsToCrouch = 0;
}

void UFBCCharacterMovementComponent::FSavedMove_FBC::SetMoveFor(ACharacter* C, float InDeltaTime,
	FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	FSavedMove_Character::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	UFBCCharacterMovementComponent* CharacterMovement = Cast<UFBCCharacterMovementComponent>(C->GetCharacterMovement());

	bPrevWantsToCrouch = CharacterMovement->bPrevWantsToCrouch;
}

void UFBCCharacterMovementComponent::FSavedMove_FBC::PrepMoveFor(ACharacter* C)
{
	FSavedMove_Character::PrepMoveFor(C);

	UFBCCharacterMovementComponent* CharacterMovement = Cast<UFBCCharacterMovementComponent>(C->GetCharacterMovement());

	CharacterMovement->bPrevWantsToCrouch = bPrevWantsToCrouch;
}

UFBCCharacterMovementComponent::FNetworkPredictionData_Client_FBC::FNetworkPredictionData_Client_FBC(
 	const UCharacterMovementComponent& ClientMovement) : Super(ClientMovement)
{}

FSavedMovePtr UFBCCharacterMovementComponent::FNetworkPredictionData_Client_FBC::AllocateNewMove()
{
	return FSavedMovePtr{new FSavedMove_FBC};
}

FNetworkPredictionData_Client* UFBCCharacterMovementComponent::GetPredictionData_Client() const
{
	check(PawnOwner != nullptr);

	if (ClientPredictionData == nullptr)
	{
		UFBCCharacterMovementComponent* MutableThis = const_cast<UFBCCharacterMovementComponent*>(this);

		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_FBC(*this);
		MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
		MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
	}
	return ClientPredictionData;
}

bool UFBCCharacterMovementComponent::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

void UFBCCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation,
	const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	bPrevWantsToCrouch = bWantsToCrouch;
}

void UFBCCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	// We just started crouching - check if we can start sliding
	if (MovementMode == MOVE_Walking && bWantsToCrouch && !bPrevWantsToCrouch)
	{
		FHitResult PotentialSlideSurface;
		if (Velocity.SizeSquared() > Slide_MinSpeed * Slide_MinSpeed && GetSlideSurface(PotentialSlideSurface))
		{
			EnterSlide();
		}
	}

	// We just stopped crouching - stop sliding
	if (IsCustomMovementMode(CMOVE_Slide) && !bWantsToCrouch)
	{
		ExitSlide();
	}

	// This is where crouching logic happens. We want to handle sliding before that 
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

void UFBCCharacterMovementComponent::PhysCustom(float DeltaTime, int32 Iterations)
{
	Super::PhysCustom(DeltaTime, Iterations);

	switch (CustomMovementMode)
	{
	case CMOVE_Slide:
		PhysSlide(DeltaTime, Iterations);
		break;
	default:
		UE_LOG(LogFBC, Fatal, TEXT("Invalid Movement Mode"));
	}
}

bool UFBCCharacterMovementComponent::IsMovingOnGround() const
{
	return Super::IsMovingOnGround() || IsCustomMovementMode(CMOVE_Slide);
}

bool UFBCCharacterMovementComponent::CanCrouchInCurrentState() const
{
	return Super::CanCrouchInCurrentState() && IsMovingOnGround();
}

void UFBCCharacterMovementComponent::EnterSlide()
{
	// Still apply crouch for reduced capsule height
	bWantsToCrouch = true;
	
	FBCCharacterOwner->bIsSliding = true;

	// Apply initial boost when entering slide
	Velocity += Velocity.GetSafeNormal2D() * Slide_EnterImpulse;

	SetMovementMode(MOVE_Custom, CMOVE_Slide);
}

void UFBCCharacterMovementComponent::ExitSlide()
{
	bWantsToCrouch = false;

	FBCCharacterOwner->bIsSliding = false;
	
	// Sliding changes rotation of capsule to align with plane. Correct this by resetting it
	FQuat NewRotation = FRotationMatrix::MakeFromXZ(UpdatedComponent->GetForwardVector().GetSafeNormal2D(), FVector::UpVector).ToQuat();
	FHitResult Hit;
	SafeMoveUpdatedComponent(FVector::ZeroVector, NewRotation, true, Hit);
	SetMovementMode(MOVE_Walking);
}

void UFBCCharacterMovementComponent::PhysSlide(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME)
	{
		return;
	}

	RestorePreAdditiveRootMotionVelocity();

	// Ensure we satisfy slide conditions
	FHitResult SurfaceHit;
	if (!GetSlideSurface(SurfaceHit) || Velocity.SizeSquared() < Slide_MinSpeed * Slide_MinSpeed)
	{
		ExitSlide();
		StartNewPhysics(DeltaTime, Iterations);
		return;
	}

	// Apply acceleration from gravity
	Velocity += Slide_GravityForce * FVector::DownVector * DeltaTime;

	// Strafe - we are accelerating orthogonally to where we're moving. Steering only
	if (FMath::Abs(FVector::DotProduct(Acceleration.GetSafeNormal(), UpdatedComponent->GetRightVector())) > .5)
	{
		Acceleration = Acceleration.ProjectOnTo(UpdatedComponent->GetRightVector());
	}
	else
	{
		Acceleration = FVector::ZeroVector;
	}

	// Calculate velocity if we're not using root motion
	if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		CalcVelocity(DeltaTime, Slide_Friction, true, GetMaxBrakingDeceleration());
	}
	ApplyRootMotionToVelocity(DeltaTime);

	// Perform the move
	++Iterations;
	bJustTeleported = false;

	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	FQuat OldRotation = UpdatedComponent->GetComponentRotation().Quaternion();
	FHitResult SweepHit(1.f);
	FVector DeltaLocation = Velocity * DeltaTime;

	// Ensure capsule stays on the surface we're sliding on
	FVector VelocityPlaneDirection = FVector::VectorPlaneProject(Velocity, SurfaceHit.Normal).GetSafeNormal();
	FQuat NewRotation = FRotationMatrix::MakeFromXZ(VelocityPlaneDirection, SurfaceHit.Normal).ToQuat();

	// Actually move the character
	SafeMoveUpdatedComponent(DeltaLocation, NewRotation, true, SweepHit);

	// We hit a wall while sliding - slide along the wall instead of stopping entirely
	if (SweepHit.Time < 1.f)
	{
		HandleImpact(SweepHit, DeltaTime, DeltaLocation);
		SlideAlongSurface(DeltaLocation, (1.f - SweepHit.Time), SweepHit.Normal, SweepHit, true);
	}

	// Ensure we satisfy slide conditions (again)
	FHitResult NewSurfaceHit;
	if (!GetSlideSurface(NewSurfaceHit) || Velocity.SizeSquared() < Slide_MinSpeed * Slide_MinSpeed)
	{
		ExitSlide();
	}

	// Keep track of velocity based on the displacement we've actually travelled
	if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / DeltaTime;
	}
}

bool UFBCCharacterMovementComponent::GetSlideSurface(FHitResult& Hit) const
{
	FVector Start = UpdatedComponent->GetComponentLocation();
	FVector End = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.f * FVector::DownVector;
	FName ProfileName = TEXT("BlockAll");
	return GetWorld()->LineTraceSingleByProfile(Hit, Start, End, ProfileName, FBCCharacterOwner->GetIgnoreCharacterParams());
}

void UFBCCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();

	FBCCharacterOwner = Cast<AFBCCharacter>(GetOwner());
}
