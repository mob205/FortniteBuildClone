// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/FBCCharacterMovementComponent.h"

#include "Components/CapsuleComponent.h"
#include "FortniteBuildClone/FortniteBuildClone.h"
#include "GameFramework/Character.h"
#include "Structure/PlacedStructure.h"

void FFBCMoveResponseDataContainer::ServerFillResponseData(const UCharacterMovementComponent& CharacterMovement,
                                                           const FClientAdjustment& PendingAdjustment)
{
	FCharacterMoveResponseDataContainer::ServerFillResponseData(CharacterMovement, PendingAdjustment);

	const UFBCCharacterMovementComponent* MoveComp = Cast<UFBCCharacterMovementComponent>(&CharacterMovement);
	bStaminaDrained = MoveComp->IsStaminaDrained();
	Stamina = MoveComp->GetStamina();
	CurrentStaminaRegenDelay = MoveComp->GetCurrentStaminaRegenDelay();
}

bool FFBCMoveResponseDataContainer::Serialize(UCharacterMovementComponent& CharacterMovementComponent, FArchive& Ar,
	UPackageMap* PackageMap)
{
	if (!Super::Serialize(CharacterMovementComponent, Ar, PackageMap))
	{
		return false;
	}

	if (IsCorrection())
	{
		Ar << Stamina;
		Ar << bStaminaDrained;
		Ar << CurrentStaminaRegenDelay;
	}

	return !Ar.IsError();
}

void FFBCNetworkMoveData::ClientFillNetworkMoveData(const FSavedMove_Character& ClientMove, ENetworkMoveType MoveType)
{
	FCharacterNetworkMoveData::ClientFillNetworkMoveData(ClientMove, MoveType);

	const FSavedMove_FBC& FBCClientMove = static_cast<const FSavedMove_FBC&>(ClientMove);
	Stamina = FBCClientMove.EndStamina;
	CurrentStaminaRegenDelay = FBCClientMove.CurrentStaminaRegenDelay;
}

bool FFBCNetworkMoveData::Serialize(UCharacterMovementComponent& CharacterMovement, FArchive& Ar,
	UPackageMap* PackageMap, ENetworkMoveType MoveType)
{
	Super::Serialize(CharacterMovement, Ar, PackageMap, MoveType);

	SerializeOptionalValue<float>(Ar.IsSaving(), Ar, Stamina, 0.f);
	SerializeOptionalValue<float>(Ar.IsSaving(), Ar, CurrentStaminaRegenDelay, 0.f);
	return !Ar.IsError();
}

void FSavedMove_FBC::Clear()
{
	FSavedMove_Character::Clear();

	bWantsToSprint = 0;
	bPrevWantsToCrouch = 0;
	bPrevWasSprinting = 0;

	bStaminaDrained = false;
	StartStamina = 0.f;
	EndStamina = 0.f;
	CurrentStaminaRegenDelay = 0.f;
}

void FSavedMove_FBC::SetMoveFor(ACharacter* C, float InDeltaTime,
	FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
	Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

	UFBCCharacterMovementComponent* CharacterMovement = Cast<UFBCCharacterMovementComponent>(C->GetCharacterMovement());

	bWantsToSprint = CharacterMovement->bWantsToSprint;
	bPrevWantsToCrouch = CharacterMovement->bPrevWantsToCrouch;
	bPrevWasSprinting = CharacterMovement->bPrevWasSprinting;
}

void FSavedMove_FBC::PrepMoveFor(ACharacter* C)
{
	FSavedMove_Character::PrepMoveFor(C);

	UFBCCharacterMovementComponent* CharacterMovement = Cast<UFBCCharacterMovementComponent>(C->GetCharacterMovement());

	CharacterMovement->bWantsToSprint = bWantsToSprint;
	CharacterMovement->bPrevWantsToCrouch = bPrevWantsToCrouch;
	CharacterMovement->bPrevWasSprinting = bPrevWasSprinting;
}

void UFBCCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	bWantsToSprint = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}

void UFBCCharacterMovementComponent::CalcVelocity(float DeltaTime, float Friction, bool bFluid,
	float BrakingDeceleration)
{
	Super::CalcVelocity(DeltaTime, Friction, bFluid, BrakingDeceleration);

	if (bWantsToSprint && !bStaminaDrained)
	{
		SetStamina(GetStamina() - StaminaDrainRate * DeltaTime);
	}
	else if (CurrentStaminaRegenDelay <= 0.f)
	{
		SetStamina(GetStamina() + StaminaRegenRate * DeltaTime);
	}
}

void UFBCCharacterMovementComponent::PerformMovement(float DeltaTime)
{
	Super::PerformMovement(DeltaTime);
}


uint8 FSavedMove_FBC::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (bWantsToSprint)
	{
		Result |= FLAG_Custom_0;
	}

	return Result;
}

bool FSavedMove_FBC::CanCombineWith(const FSavedMovePtr& NewMove,
	ACharacter* InCharacter, float MaxDelta) const
{
	FSavedMove_FBC* NewFBCMove = static_cast<FSavedMove_FBC*>(NewMove.Get());
	
	if (bWantsToSprint != NewFBCMove->bWantsToSprint)
	{
		return false;
	}

	if (bStaminaDrained != NewFBCMove->bStaminaDrained)
	{
		return false;
	}
	
	if (bPrevWasSprinting != NewFBCMove->bPrevWasSprinting)
	{
		return false;
	}

	return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void FSavedMove_FBC::CombineWith(const FSavedMove_Character* OldMove, ACharacter* InCharacter, APlayerController* PC,
	const FVector& OldStartLocation)
{
	FSavedMove_Character::CombineWith(OldMove, InCharacter, PC, OldStartLocation);

	const FSavedMove_FBC* SavedOldMove = static_cast<const FSavedMove_FBC*>(OldMove);

	if (UFBCCharacterMovementComponent* CMC = InCharacter ? Cast<UFBCCharacterMovementComponent>(InCharacter->GetCharacterMovement()) : nullptr)
	{
		CMC->SetStamina(SavedOldMove->StartStamina);
		CMC->SetStaminaDrained(SavedOldMove->bStaminaDrained);
	}
}

void FSavedMove_FBC::SetInitialPosition(ACharacter* C)
{
	Super::SetInitialPosition(C);

	if (const UFBCCharacterMovementComponent* MoveComp = C ? Cast<UFBCCharacterMovementComponent>(C->GetCharacterMovement()) : nullptr)
	{
		bStaminaDrained = MoveComp->IsStaminaDrained();
		StartStamina = MoveComp->GetStamina();
		CurrentStaminaRegenDelay = MoveComp->GetCurrentStaminaRegenDelay();
	}
}

void FSavedMove_FBC::PostUpdate(ACharacter* C, EPostUpdateMode PostUpdateMode)
{
	if (UFBCCharacterMovementComponent* MoveComp = C ? Cast<UFBCCharacterMovementComponent>(C->GetCharacterMovement()) : nullptr)
	{
		EndStamina = MoveComp->GetStamina();

		if (PostUpdateMode == PostUpdate_Record)
		{
			if (bStaminaDrained != MoveComp->IsStaminaDrained())
			{
				bForceNoCombine = true;
			}
		}
	}
	FSavedMove_Character::PostUpdate(C, PostUpdateMode);
}

FNetworkPredictionData_Client_FBC::FNetworkPredictionData_Client_FBC(
 	const UCharacterMovementComponent& ClientMovement) : Super(ClientMovement)
{}

FSavedMovePtr FNetworkPredictionData_Client_FBC::AllocateNewMove()
{
	return FSavedMovePtr{new FSavedMove_FBC};
}

UFBCCharacterMovementComponent::UFBCCharacterMovementComponent()
{
	SetMoveResponseDataContainer(FBCMoveResponseDataContainer);
	SetNetworkMoveDataContainer(FBCNetworkMoveDataContainer);

	NetworkStaminaCorrectionThreshold = 2.f;
}

void UFBCCharacterMovementComponent::OnClientCorrectionReceived(
	class FNetworkPredictionData_Client_Character& ClientData, float TimeStamp, FVector NewLocation,
	FVector NewVelocity, UPrimitiveComponent* NewBase, FName NewBaseBoneName, bool bHasBase, bool bBaseRelativePosition,
	uint8 ServerMovementMode, FVector ServerGravityDirection)
{
	const FFBCMoveResponseDataContainer& MoveResponse = static_cast<const FFBCMoveResponseDataContainer&>(GetMoveResponseDataContainer());

	SetStamina(MoveResponse.Stamina);
	SetStaminaDrained(MoveResponse.bStaminaDrained);
	SetCurrentStaminaRegenDelay(MoveResponse.CurrentStaminaRegenDelay);
	
	Super::OnClientCorrectionReceived(ClientData, TimeStamp, NewLocation, NewVelocity, NewBase, NewBaseBoneName,
	                                  bHasBase, bBaseRelativePosition,
	                                  ServerMovementMode, ServerGravityDirection);
}

bool UFBCCharacterMovementComponent::ServerCheckClientError(float ClientTimeStamp, float DeltaTime,
	const FVector& Accel, const FVector& ClientWorldLocation, const FVector& RelativeClientLocation,
	UPrimitiveComponent* ClientMovementBase, FName ClientBaseBoneName, uint8 ClientMovementMode)
{
	if (Super::ServerCheckClientError(ClientTimeStamp, DeltaTime, Accel, ClientWorldLocation, RelativeClientLocation, ClientMovementBase, ClientBaseBoneName, ClientMovementMode))
	{
		return true;
	}

	// Check for desyncs above tolerance threshold
	const FFBCNetworkMoveData* CurrentMoveData = static_cast<const FFBCNetworkMoveData*>(GetCurrentNetworkMoveData());
	if (!FMath::IsNearlyEqual(CurrentMoveData->Stamina, Stamina, NetworkStaminaCorrectionThreshold))
	{
		return true;
	}

	// if (!FMath::IsNearlyEqual(CurrentMoveData->CurrentStaminaRegenDelay, CurrentStaminaRegenDelay, .1))
	// {
	// 	return true;
	// }
	return false;
}

void UFBCCharacterMovementComponent::SetStamina(float NewStamina)
{
	const float PrevStamina = Stamina;
	Stamina = FMath::Clamp(NewStamina, 0.f, MaxStamina);
	if (CharacterOwner != nullptr)
	{
		if (!FMath::IsNearlyEqual(PrevStamina, Stamina))
		{
			OnStaminaChanged(PrevStamina, Stamina);
		}
	}
}

void UFBCCharacterMovementComponent::SetMaxStamina(float NewMaxStamina)
{
	const float PrevMaxStamina = MaxStamina;
	MaxStamina = FMath::Max(0.f, NewMaxStamina);
	if (CharacterOwner != nullptr)
	{
		if (!FMath::IsNearlyEqual(PrevMaxStamina, MaxStamina))
		{
			OnMaxStaminaChanged(PrevMaxStamina, MaxStamina);
		}
	}
}

void UFBCCharacterMovementComponent::SetCurrentStaminaRegenDelay(float RegenDelay)
{
	CurrentStaminaRegenDelay = RegenDelay;
}

void UFBCCharacterMovementComponent::SetStaminaDrained(bool bNewValue)
{
	const bool bWasStaminaDrained = bStaminaDrained;
	bStaminaDrained = bNewValue;
	if (IsValid(CharacterOwner))
	{
		if (bWasStaminaDrained != bStaminaDrained)
		{
			if (bStaminaDrained)
			{
				OnStaminaDrained();
			}
			else
			{
				OnStaminaDrainRecovered();
			}
		}
	}
}

void UFBCCharacterMovementComponent::OnStaminaDrained()
{
	SetCurrentStaminaRegenDelay(StaminaRegenDelay);

	ToggleWantsToSprint(false);
}

void UFBCCharacterMovementComponent::OnStaminaDrainRecovered()
{
}

void UFBCCharacterMovementComponent::OnStaminaChanged(float PrevValue, float NewValue)
{
	if (FMath::IsNearlyZero(Stamina))
	{
		Stamina = 0.f;
		if (!bStaminaDrained)
		{
			SetStaminaDrained(true);
		}
	}
	else if (bStaminaDrained && Stamina >= MaxStamina * 0.25f)
	{
		SetStaminaDrained(false);
	}
}

void UFBCCharacterMovementComponent::OnMaxStaminaChanged(float PrevValue, float NewValue)
{
	SetStamina(GetStamina());
}

void UFBCCharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// From testing, TickComponent seems to be the only place where DeltaTime is actually reliably framerate independent
	if (CurrentStaminaRegenDelay > 0)
	{
		CurrentStaminaRegenDelay -= DeltaTime;
	}
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

void UFBCCharacterMovementComponent::ToggleWantsToSprint(bool bNewWantsToSprint)
{
	bWantsToSprint = bNewWantsToSprint;
}

void UFBCCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation,
                                                       const FVector& OldVelocity)
{
	Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

	const bool bIsCurrentlySprinting = IsSprinting();

	// We just stopped sprinting
	if (bPrevWasSprinting && !bIsCurrentlySprinting)
	{
		SetCurrentStaminaRegenDelay(StaminaRegenDelay);
	}
	bPrevWantsToCrouch = bWantsToCrouch;
	bPrevWasSprinting = bIsCurrentlySprinting;
}

float UFBCCharacterMovementComponent::GetWalkSpeed() const
{
	if (IsSprinting())
	{
		return SprintSpeeds.X;
	}
	return RunSpeeds.X;
	// const FRotator Rotation = CharacterOwner->GetActorRotation();
	//
	// float VelocityAngle{};
	//
	// if (!Velocity.IsNearlyZero())
	// {
	// 	const FMatrix RotMatrix = FRotationMatrix(Rotation);
	// 	const FVector ForwardVector = RotMatrix.GetScaledAxis(EAxis::X);
	// 	const FVector RightVector = RotMatrix.GetScaledAxis(EAxis::Y);
	// 	const FVector NormalizedVel = Velocity.GetSafeNormal2D();
	//
	// 	// get a cos(alpha) of forward vector vs velocity
	// 	const float ForwardCosAngle = static_cast<float>(FVector::DotProduct(ForwardVector, NormalizedVel));
	// 	// now get the alpha and convert to degree
	// 	float ForwardDeltaDegree = FMath::RadiansToDegrees(FMath::Acos(ForwardCosAngle));
	//
	// 	// depending on where right vector is, flip it
	// 	const float RightCosAngle = static_cast<float>(FVector::DotProduct(RightVector, NormalizedVel));
	// 	if (RightCosAngle < 0.f)
	// 	{
	// 		ForwardDeltaDegree *= -1.f;
	// 	}
	//
	// 	VelocityAngle = FMath::Abs(ForwardDeltaDegree);
	// }
	//
	// float StrafeSpeedMap = StrafeSpeedMapCurve->GetFloatValue(VelocityAngle);
	//
	// FVector Speeds = RunSpeeds;
	// if (bWantsToSprint && bCanSprint)
	// {
	// 	Speeds = SprintSpeeds;
	// }
	//
	// if (StrafeSpeedMap < 1.f)
	// {
	// 	return FMath::GetMappedRangeValueClamped(FVector2D{0, 1}, FVector2D{Speeds.X, Speeds.Y}, StrafeSpeedMap);
	// }
	// else
	// {
	// 	return FMath::GetMappedRangeValueClamped(FVector2D{1, 2}, FVector2D{Speeds.Y, Speeds.Z}, StrafeSpeedMap);
	// }
}

float UFBCCharacterMovementComponent::GetMaxAcceleration() const
{
	if (IsSprinting())
	{
		return SprintAccelerationRange.Y;
		//FVector Velocity2d = { Velocity.X, Velocity.Y, 0};
		//return FMath::GetMappedRangeValueClamped({RunSpeeds.Z, SprintSpeeds.X}, SprintAccelerationRange, Velocity2d.Length());
	}
	return WalkAcceleration;
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
	else if (MovementMode == MOVE_Walking)
	{
		MaxWalkSpeed = GetWalkSpeed();
		MaxAcceleration = GetMaxAcceleration();
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

void UFBCCharacterMovementComponent::SetBase(UPrimitiveComponent* NewBase, const FName BoneName, bool bNotifyActor)
{
	// This project currently doesn't need base functionality
	// However, it can interfere with building prediction, since the base must be replicated
	if (NewBase && NewBase->GetOwner() && NewBase->GetOwner()->GetIsReplicated())
	{
		Super::SetBase(NewBase, BoneName, bNotifyActor);
	}
	else
	{
		Super::SetBase(nullptr, {}, bNotifyActor);
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
		Acceleration = .5 * Acceleration.ProjectOnTo(UpdatedComponent->GetRightVector());
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
	SetMaxStamina(DefaultMaxStamina);
	SetStamina(GetMaxStamina());
}
