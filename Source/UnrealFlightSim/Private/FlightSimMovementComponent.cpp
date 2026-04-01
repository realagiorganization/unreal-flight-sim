#include "FlightSimMovementComponent.h"

#include "CollisionQueryParams.h"
#include "Engine/EngineTypes.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

UFlightSimMovementComponent::UFlightSimMovementComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UFlightSimMovementComponent::BeginPlay()
{
    Super::BeginPlay();
    AirspeedMps = FMath::Clamp(AirspeedMps, MinSpeedMps, MaxSpeedMps);
    GroundSpeedMps = AirspeedMps;
    CurrentWindWorldMps = BaseWindWorldMps;
}

void UFlightSimMovementComponent::TickComponent(
    float DeltaTime,
    ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction
)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    AActor* Owner = GetOwner();
    if (!Owner || DeltaTime <= KINDA_SMALL_NUMBER)
    {
        return;
    }

    UpdateEnvironment(Owner);

    ThrottlePercent = FMath::Clamp(ThrottlePercent + (ThrottleInput * ThrottleResponse * DeltaTime), 0.0f, 1.0f);

    const float DesiredSpeed = FMath::Lerp(MinSpeedMps, MaxSpeedMps, ThrottlePercent);
    AirspeedMps = FMath::FInterpTo(AirspeedMps, DesiredSpeed, DeltaTime, SpeedResponse);

    const float SafeMaxBank = FMath::Max(MaxBankAngleDegrees, 1.0f);
    const float BankLoadPenalty = FMath::Abs(BankAngleDegrees) / SafeMaxBank;
    const float PitchLoadPenalty = FMath::Max(PitchInput, 0.0f) * 0.2f;
    const float EffectiveStallSpeed = StallSpeedMps * (1.0f + (BankLoadPenalty * 0.35f) + PitchLoadPenalty);
    const float StallRatio = AirspeedMps / FMath::Max(EffectiveStallSpeed, 1.0f);
    StallSeverityValue = FMath::Clamp((1.0f - StallRatio) / 0.28f, 0.0f, 1.0f);

    const float ControlAuthority = FMath::Lerp(1.0f, 1.0f - StallAuthorityLoss, StallSeverityValue);
    const float DesiredBank = RollInput * MaxBankAngleDegrees * ControlAuthority;
    BankAngleDegrees = FMath::FInterpTo(BankAngleDegrees, DesiredBank, DeltaTime, BankResponse);

    const FRotator CurrentRotation = Owner->GetActorRotation();
    const FVector ForwardVector = Owner->GetActorForwardVector();
    const FVector RightVector = Owner->GetActorRightVector();
    const float CrossWindMps = FVector::DotProduct(CurrentWindWorldMps, RightVector);
    const float WeathercockYawDegrees =
        (CrossWindMps / FMath::Max(AirspeedMps, 1.0f)) * WeathercockYawRateDegrees * ControlAuthority;

    const float NewPitch = FMath::Clamp(
        CurrentRotation.Pitch + (PitchInput * MaxPitchRateDegrees * ControlAuthority * DeltaTime),
        -35.0f,
        40.0f
    );
    const float NewYaw = CurrentRotation.Yaw
        + ((YawInput * MaxYawRateDegrees * ControlAuthority)
            + ((BankAngleDegrees / SafeMaxBank) * BankTurnRateDegrees)
            + WeathercockYawDegrees)
            * DeltaTime;
    const float NewRoll = FMath::FInterpTo(CurrentRotation.Roll, -BankAngleDegrees, DeltaTime, RollVisualResponse);
    const FRotator NewRotation(NewPitch, NewYaw, NewRoll);

    Owner->SetActorRotation(NewRotation);

    const float GroundEffectAlpha =
        1.0f - FMath::Clamp(AltitudeAboveGroundCm / FMath::Max(GroundEffectHeightCm, 1.0f), 0.0f, 1.0f);
    const float LiftAssistMps = GroundEffectAlpha * GroundEffectLiftMps * (1.0f - StallSeverityValue);
    const float StallSinkMps = StallSeverityValue * StallSinkRateMps;

    const FVector AirVelocity = ForwardVector * AirspeedMps;
    const FVector GroundVelocity = AirVelocity + CurrentWindWorldMps;
    GroundSpeedMps = FVector(GroundVelocity.X, GroundVelocity.Y, 0.0f).Size();

    FVector DeltaLocation = GroundVelocity * DeltaTime * 100.0f;
    DeltaLocation.Z += (
        (PitchInput * 1800.0f * ControlAuthority)
        + (ThrottlePercent * CruiseLiftMps * 100.0f)
        + (LiftAssistMps * 100.0f)
        - (StallSinkMps * 100.0f))
        * DeltaTime;

    FVector NextLocation = Owner->GetActorLocation() + DeltaLocation;
    NextLocation.Z = FMath::Max(NextLocation.Z, MinimumAltitudeCm);
    Owner->SetActorLocation(NextLocation, true);
}

void UFlightSimMovementComponent::SetThrottleInput(float Value)
{
    ThrottleInput = FMath::Clamp(Value, -1.0f, 1.0f);
}

void UFlightSimMovementComponent::SetPitchInput(float Value)
{
    PitchInput = FMath::Clamp(Value, -1.0f, 1.0f);
}

void UFlightSimMovementComponent::SetRollInput(float Value)
{
    RollInput = FMath::Clamp(Value, -1.0f, 1.0f);
}

void UFlightSimMovementComponent::SetYawInput(float Value)
{
    YawInput = FMath::Clamp(Value, -1.0f, 1.0f);
}

void UFlightSimMovementComponent::ResetFlightState()
{
    AirspeedMps = 95.0f;
    GroundSpeedMps = AirspeedMps;
    ThrottlePercent = 0.55f;
    ThrottleInput = 0.0f;
    PitchInput = 0.0f;
    RollInput = 0.0f;
    YawInput = 0.0f;
    BankAngleDegrees = 0.0f;
    AltitudeAboveGroundCm = TerrainAwarenessHeightCm;
    TerrainProximity = 0.0f;
    StallSeverityValue = 0.0f;
    CurrentWindWorldMps = BaseWindWorldMps;
}

float UFlightSimMovementComponent::GetAirspeedKnots() const
{
    return AirspeedMps * 1.94384f;
}

float UFlightSimMovementComponent::GetThrottlePercent() const
{
    return ThrottlePercent;
}

float UFlightSimMovementComponent::GetGroundSpeedKnots() const
{
    return GroundSpeedMps * 1.94384f;
}

float UFlightSimMovementComponent::GetAltitudeAboveGroundMeters() const
{
    return AltitudeAboveGroundCm / 100.0f;
}

float UFlightSimMovementComponent::GetTerrainProximityAlpha() const
{
    return TerrainProximity;
}

float UFlightSimMovementComponent::GetStallSeverity() const
{
    return StallSeverityValue;
}

FVector UFlightSimMovementComponent::GetWindVectorWorldMps() const
{
    return CurrentWindWorldMps;
}

void UFlightSimMovementComponent::UpdateEnvironment(const AActor* Owner)
{
    UWorld* World = GetWorld();
    if (!World || !Owner)
    {
        CurrentWindWorldMps = BaseWindWorldMps;
        AltitudeAboveGroundCm = TerrainAwarenessHeightCm;
        TerrainProximity = 0.0f;
        return;
    }

    const float AngularFrequency = World->GetTimeSeconds() * GustFrequencyHz * UE_TWO_PI;
    const FVector GustVector(
        FMath::Sin(AngularFrequency) * GustAmplitudeMps,
        FMath::Cos(AngularFrequency * 1.37f) * GustAmplitudeMps * 0.6f,
        FMath::Sin(AngularFrequency * 2.13f) * GustVerticalAmplitudeMps
    );
    CurrentWindWorldMps = BaseWindWorldMps + GustVector;

    FHitResult GroundHit;
    FCollisionQueryParams QueryParams(TEXT("BushFlightGroundProbe"), true, Owner);
    const FVector Start = Owner->GetActorLocation();
    const FVector End = Start - (FVector::UpVector * TerrainProbeLengthCm);

    if (World->LineTraceSingleByChannel(GroundHit, Start, End, ECC_Visibility, QueryParams))
    {
        AltitudeAboveGroundCm = GroundHit.Distance;
    }
    else
    {
        AltitudeAboveGroundCm = TerrainProbeLengthCm;
    }

    TerrainProximity =
        1.0f - FMath::Clamp(AltitudeAboveGroundCm / FMath::Max(TerrainAwarenessHeightCm, 1.0f), 0.0f, 1.0f);
}
