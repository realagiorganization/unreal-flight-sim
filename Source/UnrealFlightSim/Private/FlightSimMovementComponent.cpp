#include "FlightSimMovementComponent.h"

#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"
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
    VerticalSpeedMps = 0.0f;
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

    if (bIsGrounded)
    {
        UpdateGroundRoll(Owner, DeltaTime);
        return;
    }

    ThrottlePercent = FMath::Clamp(ThrottlePercent + (ThrottleInput * ThrottleResponse * DeltaTime), 0.0f, 1.0f);

    const float DesiredSpeed = FMath::Lerp(MinSpeedMps, MaxSpeedMps, ThrottlePercent);
    AirspeedMps = FMath::FInterpTo(AirspeedMps, DesiredSpeed, DeltaTime, SpeedResponse);
    const float PayloadRatio = GetPayloadRatio();

    const float SafeMaxBank = FMath::Max(MaxBankAngleDegrees, 1.0f);
    const float BankLoadPenalty = FMath::Abs(BankAngleDegrees) / SafeMaxBank;
    const float PitchLoadPenalty = FMath::Max(PitchInput, 0.0f) * 0.2f;
    const float EffectiveStallSpeed =
        StallSpeedMps * (1.0f + (PayloadRatio * PayloadStallPenalty) + (BankLoadPenalty * 0.35f) + PitchLoadPenalty);
    const float StallRatio = AirspeedMps / FMath::Max(EffectiveStallSpeed, 1.0f);
    StallSeverityValue = FMath::Clamp((1.0f - StallRatio) / 0.28f, 0.0f, 1.0f);

    const float StallLimitedControlAuthority = FMath::Lerp(1.0f, 1.0f - StallAuthorityLoss, StallSeverityValue);
    const float ControlAuthority = StallLimitedControlAuthority * FMath::Lerp(1.0f, 1.0f - PayloadAuthorityPenalty, PayloadRatio);
    const float DesiredBank = RollInput * MaxBankAngleDegrees * ControlAuthority;
    BankAngleDegrees = FMath::FInterpTo(BankAngleDegrees, DesiredBank, DeltaTime, BankResponse);

    const FRotator CurrentRotation = Owner->GetActorRotation();
    const FVector RightVector = Owner->GetActorRightVector();
    const float CrossWindMps = FVector::DotProduct(CurrentWindWorldMps, RightVector);
    const float WeathercockYawDegrees =
        (CrossWindMps / FMath::Max(AirspeedMps, 1.0f)) * WeathercockYawRateDegrees * ControlAuthority;

    const float NewPitch = FMath::Clamp(
        CurrentRotation.Pitch + (PitchInput * MaxPitchRateDegrees * ControlAuthority * DeltaTime),
        -30.0f,
        35.0f
    );
    const float NewYaw = CurrentRotation.Yaw
        + ((YawInput * MaxYawRateDegrees * ControlAuthority)
            + ((BankAngleDegrees / SafeMaxBank) * BankTurnRateDegrees)
            + WeathercockYawDegrees)
            * DeltaTime;
    const float NewRoll = FMath::FInterpTo(CurrentRotation.Roll, -BankAngleDegrees, DeltaTime, RollVisualResponse);
    const FRotator NewRotation(NewPitch, NewYaw, NewRoll);
    const FVector FlightForwardVector = NewRotation.Vector();

    Owner->SetActorRotation(NewRotation);

    const float GroundEffectAlpha =
        1.0f - FMath::Clamp(AltitudeAboveGroundCm / FMath::Max(GroundEffectHeightCm, 1.0f), 0.0f, 1.0f);
    const float LiftAssistMps = GroundEffectAlpha * GroundEffectLiftMps * (1.0f - StallSeverityValue);
    const float StallSinkMps = StallSeverityValue * StallSinkRateMps;
    const float PayloadLiftPenaltyMps = PayloadRatio * CruiseLiftMps * PayloadLiftPenalty;

    const FVector AirVelocity = FlightForwardVector * AirspeedMps;
    const FVector GroundVelocity = AirVelocity + CurrentWindWorldMps;
    GroundSpeedMps = FVector(GroundVelocity.X, GroundVelocity.Y, 0.0f).Size();

    FVector DeltaLocation = GroundVelocity * DeltaTime * 100.0f;
    DeltaLocation.Z += (
        (PitchInput * 1200.0f * ControlAuthority)
        + ((ThrottlePercent * CruiseLiftMps) - PayloadLiftPenaltyMps) * 100.0f
        + (LiftAssistMps * 100.0f)
        - (StallSinkMps * 100.0f))
        * DeltaTime;
    VerticalSpeedMps = DeltaLocation.Z / (100.0f * DeltaTime);

    FVector NextLocation = Owner->GetActorLocation() + DeltaLocation;
    if (bHasGroundReference && NextLocation.Z <= (GroundHeightCm + TouchdownClearanceCm))
    {
        NextLocation.Z = GroundHeightCm + TouchdownClearanceCm;
        FinalizeTouchdown(Owner, NextLocation, NewRotation);
        return;
    }

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

void UFlightSimMovementComponent::SetPayloadWeightKg(float Value)
{
    PayloadWeightKg = FMath::Clamp(Value, 0.0f, FMath::Max(MaxPayloadWeightKg, 0.0f));
}

void UFlightSimMovementComponent::ResetFlightState()
{
    AirspeedMps = 30.0f;
    GroundSpeedMps = AirspeedMps;
    VerticalSpeedMps = 0.0f;
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
    bHasGroundReference = false;
    bIsGrounded = false;
    bLandingEvaluationReady = false;
    bLandingWasCrash = false;
    bLandingWasOverrun = false;
    GroundHeightCm = 0.0f;
    StripTravelDirection = 1.0f;
    LandingScorePercent = 0.0f;
    LandingRating = TEXT("In Flight");
    TouchdownSpeedMps = 0.0f;
    TouchdownSinkRateMps = 0.0f;
    LandingRollDistanceCm = 0.0f;
    RunwayRemainingCm = 0.0f;
    RunwayAlignmentErrorDegrees = 0.0f;
    RunwayCenterDeviationCm = 0.0f;
}

float UFlightSimMovementComponent::GetAirspeedKnots() const
{
    return AirspeedMps * 1.94384f;
}

float UFlightSimMovementComponent::GetThrottlePercent() const
{
    return ThrottlePercent;
}

float UFlightSimMovementComponent::GetPayloadWeightKg() const
{
    return PayloadWeightKg;
}

float UFlightSimMovementComponent::GetPayloadRatio() const
{
    return FMath::Clamp(PayloadWeightKg / FMath::Max(MaxPayloadWeightKg, 1.0f), 0.0f, 1.0f);
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

float UFlightSimMovementComponent::GetVerticalSpeedMps() const
{
    return VerticalSpeedMps;
}

float UFlightSimMovementComponent::GetStallSeverity() const
{
    return StallSeverityValue;
}

FVector UFlightSimMovementComponent::GetWindVectorWorldMps() const
{
    return CurrentWindWorldMps;
}

bool UFlightSimMovementComponent::IsGrounded() const
{
    return bIsGrounded;
}

bool UFlightSimMovementComponent::HasLandingEvaluation() const
{
    return bLandingEvaluationReady;
}

float UFlightSimMovementComponent::GetLandingScorePercent() const
{
    return LandingScorePercent;
}

FString UFlightSimMovementComponent::GetLandingRating() const
{
    return LandingRating;
}

float UFlightSimMovementComponent::GetTouchdownSpeedKnots() const
{
    return TouchdownSpeedMps * 1.94384f;
}

float UFlightSimMovementComponent::GetTouchdownSinkRateMps() const
{
    return TouchdownSinkRateMps;
}

float UFlightSimMovementComponent::GetLandingRollDistanceMeters() const
{
    return LandingRollDistanceCm / 100.0f;
}

float UFlightSimMovementComponent::GetRunwayRemainingMeters() const
{
    return RunwayRemainingCm / 100.0f;
}

float UFlightSimMovementComponent::GetRunwayAlignmentErrorDegrees() const
{
    return RunwayAlignmentErrorDegrees;
}

float UFlightSimMovementComponent::GetRunwayCenterDeviationMeters() const
{
    return RunwayCenterDeviationCm / 100.0f;
}

bool UFlightSimMovementComponent::WasLandingCrash() const
{
    return bLandingWasCrash;
}

bool UFlightSimMovementComponent::WasLandingOverrun() const
{
    return bLandingWasOverrun;
}

void UFlightSimMovementComponent::UpdateEnvironment(const AActor* Owner)
{
    UWorld* World = GetWorld();
    if (!World || !Owner)
    {
        CurrentWindWorldMps = BaseWindWorldMps;
        AltitudeAboveGroundCm = TerrainAwarenessHeightCm;
        TerrainProximity = 0.0f;
        bHasGroundReference = bUseVirtualPracticeStrip;
        GroundHeightCm = PracticeStripCenter.Z;
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
        bHasGroundReference = true;
        GroundHeightCm = GroundHit.ImpactPoint.Z;
        AltitudeAboveGroundCm = GroundHit.Distance;
    }
    else if (bUseVirtualPracticeStrip)
    {
        bHasGroundReference = true;
        GroundHeightCm = PracticeStripCenter.Z;
        AltitudeAboveGroundCm = FMath::Max(Start.Z - GroundHeightCm, 0.0f);
    }
    else
    {
        bHasGroundReference = false;
        GroundHeightCm = Start.Z - TerrainProbeLengthCm;
        AltitudeAboveGroundCm = TerrainProbeLengthCm;
    }

    TerrainProximity =
        1.0f - FMath::Clamp(AltitudeAboveGroundCm / FMath::Max(TerrainAwarenessHeightCm, 1.0f), 0.0f, 1.0f);

    if (bUseVirtualPracticeStrip)
    {
        const FVector StripForward = GetPracticeStripForwardVector();
        const float GuidanceHeadingSign = FVector::DotProduct(Owner->GetActorForwardVector(), StripForward) >= 0.0f ? 1.0f : -1.0f;
        float LongitudinalCm = 0.0f;
        float LateralCm = 0.0f;
        float RemainingCm = 0.0f;

        if (GetStripFrameMetrics(Owner->GetActorLocation(), GuidanceHeadingSign, LongitudinalCm, LateralCm, RemainingCm))
        {
            const float GuidanceHeadingDegrees =
                PracticeStripHeadingDegrees + (GuidanceHeadingSign < 0.0f ? 180.0f : 0.0f);
            RunwayRemainingCm = RemainingCm;
            RunwayCenterDeviationCm = FMath::Abs(LateralCm);
            RunwayAlignmentErrorDegrees =
                FMath::Abs(FMath::FindDeltaAngleDegrees(Owner->GetActorRotation().Yaw, GuidanceHeadingDegrees));
        }
    }

    DrawPracticeStripDebug(World);
}

FVector UFlightSimMovementComponent::GetPracticeStripForwardVector() const
{
    return FRotator(0.0f, PracticeStripHeadingDegrees, 0.0f).Vector();
}

FVector UFlightSimMovementComponent::GetPracticeStripRightVector() const
{
    return FRotationMatrix(FRotator(0.0f, PracticeStripHeadingDegrees, 0.0f)).GetUnitAxis(EAxis::Y);
}

bool UFlightSimMovementComponent::GetStripFrameMetrics(
    const FVector& WorldLocation,
    float HeadingSign,
    float& OutLongitudinalCm,
    float& OutLateralCm,
    float& OutRunwayRemainingCm
) const
{
    if (!bUseVirtualPracticeStrip)
    {
        OutLongitudinalCm = 0.0f;
        OutLateralCm = 0.0f;
        OutRunwayRemainingCm = 0.0f;
        return false;
    }

    const FVector StripForward = GetPracticeStripForwardVector() * HeadingSign;
    const FVector StripRight = GetPracticeStripRightVector() * HeadingSign;
    const FVector StripOffset = WorldLocation - PracticeStripCenter;
    const float HalfStripLengthCm = PracticeStripLengthCm * 0.5f;

    OutLongitudinalCm = FVector::DotProduct(StripOffset, StripForward);
    OutLateralCm = FVector::DotProduct(StripOffset, StripRight);
    OutRunwayRemainingCm = HalfStripLengthCm - OutLongitudinalCm;
    return true;
}

void UFlightSimMovementComponent::FinalizeTouchdown(
    AActor* Owner,
    const FVector& TouchdownLocation,
    const FRotator& TouchdownRotation
)
{
    if (!Owner)
    {
        return;
    }

    const FVector StripForward = GetPracticeStripForwardVector();
    const FVector ForwardVector = TouchdownRotation.Vector();
    StripTravelDirection = FVector::DotProduct(ForwardVector, StripForward) >= 0.0f ? 1.0f : -1.0f;

    float LongitudinalCm = 0.0f;
    float LateralCm = 0.0f;
    float RemainingCm = 0.0f;
    GetStripFrameMetrics(TouchdownLocation, StripTravelDirection, LongitudinalCm, LateralCm, RemainingCm);

    const float StripHeadingDegrees = PracticeStripHeadingDegrees + (StripTravelDirection < 0.0f ? 180.0f : 0.0f);
    const float HeadingDelta = FMath::FindDeltaAngleDegrees(TouchdownRotation.Yaw, StripHeadingDegrees);
    const float TouchdownBankDegrees = FMath::Abs(TouchdownRotation.Roll);

    bIsGrounded = true;
    bLandingEvaluationReady = true;
    bLandingWasOverrun = false;
    TouchdownSpeedMps = GroundSpeedMps;
    TouchdownSinkRateMps = FMath::Max(-VerticalSpeedMps, 0.0f);
    LandingRollDistanceCm = 0.0f;
    RunwayRemainingCm = RemainingCm;
    RunwayAlignmentErrorDegrees = FMath::Abs(HeadingDelta);
    RunwayCenterDeviationCm = FMath::Abs(LateralCm);
    bLandingWasCrash = TouchdownSinkRateMps > (MaxSafeTouchdownSinkRateMps * 2.0f)
        || TouchdownSpeedMps > (MaxSafeTouchdownSpeedMps * 1.45f)
        || TouchdownBankDegrees > (MaxSafeTouchdownBankDegrees * 1.75f)
        || RunwayCenterDeviationCm > (PracticeStripWidthCm * 0.75f);

    AirspeedMps = GroundSpeedMps;
    VerticalSpeedMps = 0.0f;
    AltitudeAboveGroundCm = 0.0f;
    TerrainProximity = 1.0f;
    Owner->SetActorLocation(TouchdownLocation, false);
    Owner->SetActorRotation(FRotator(0.0f, TouchdownRotation.Yaw, 0.0f));
    UpdateLandingEvaluation();
}

void UFlightSimMovementComponent::UpdateGroundRoll(AActor* Owner, float DeltaTime)
{
    if (!Owner)
    {
        return;
    }

    const float RollDistanceCm = GroundSpeedMps * DeltaTime * 100.0f;
    LandingRollDistanceCm += RollDistanceCm;

    const float PayloadAdjustedDeceleration = GroundRollDecelerationMps2
        * FMath::Lerp(1.0f, 1.0f - PayloadGroundRollPenalty, GetPayloadRatio());
    const float DecelerationMps2 = bLandingWasCrash ? CrashGroundDecelerationMps2 : PayloadAdjustedDeceleration;
    GroundSpeedMps = FMath::Max(0.0f, GroundSpeedMps - (DecelerationMps2 * DeltaTime));
    AirspeedMps = GroundSpeedMps;
    VerticalSpeedMps = 0.0f;
    StallSeverityValue = 0.0f;
    AltitudeAboveGroundCm = 0.0f;
    TerrainProximity = 1.0f;

    const FVector ForwardVector = Owner->GetActorForwardVector();
    FVector NextLocation = Owner->GetActorLocation() + (ForwardVector * RollDistanceCm);
    NextLocation.Z = GroundHeightCm + TouchdownClearanceCm;
    Owner->SetActorLocation(NextLocation, false);

    float LongitudinalCm = 0.0f;
    float LateralCm = 0.0f;
    float RemainingCm = 0.0f;
    if (GetStripFrameMetrics(NextLocation, StripTravelDirection, LongitudinalCm, LateralCm, RemainingCm))
    {
        RunwayRemainingCm = RemainingCm;
        RunwayCenterDeviationCm = FMath::Abs(LateralCm);

        if (RunwayRemainingCm < 0.0f && GroundSpeedMps > GroundStopSpeedMps)
        {
            bLandingWasOverrun = true;
        }
    }

    if (GroundSpeedMps <= GroundStopSpeedMps)
    {
        GroundSpeedMps = 0.0f;
        AirspeedMps = 0.0f;
    }

    UpdateLandingEvaluation();
}

void UFlightSimMovementComponent::UpdateLandingEvaluation()
{
    if (!bLandingEvaluationReady)
    {
        return;
    }

    if (bLandingWasCrash)
    {
        LandingScorePercent = 18.0f;
        LandingRating = TEXT("Crash");
        return;
    }

    float Score = 100.0f;
    Score -= FMath::GetMappedRangeValueClamped(
        FVector2D(MaxSafeTouchdownSpeedMps, MaxSafeTouchdownSpeedMps * 1.5f),
        FVector2D(0.0f, 35.0f),
        TouchdownSpeedMps
    );
    Score -= FMath::GetMappedRangeValueClamped(
        FVector2D(MaxSafeTouchdownSinkRateMps, MaxSafeTouchdownSinkRateMps * 2.2f),
        FVector2D(0.0f, 35.0f),
        TouchdownSinkRateMps
    );
    Score -= FMath::GetMappedRangeValueClamped(
        FVector2D(4.0f, 28.0f),
        FVector2D(0.0f, 18.0f),
        RunwayAlignmentErrorDegrees
    );
    Score -= FMath::GetMappedRangeValueClamped(
        FVector2D(75.0f, (PracticeStripWidthCm * 0.5f) + 250.0f),
        FVector2D(0.0f, 12.0f),
        RunwayCenterDeviationCm
    );

    if (bLandingWasOverrun)
    {
        Score -= 28.0f;
    }

    Score -= FMath::GetMappedRangeValueClamped(
        FVector2D(25.0f, 0.0f),
        FVector2D(0.0f, 12.0f),
        RunwayRemainingCm / 100.0f
    );

    LandingScorePercent = FMath::Clamp(Score, 0.0f, 100.0f);

    if (bLandingWasOverrun)
    {
        LandingRating = TEXT("Overrun");
    }
    else if (LandingScorePercent >= 90.0f)
    {
        LandingRating = TEXT("Excellent");
    }
    else if (LandingScorePercent >= 78.0f)
    {
        LandingRating = TEXT("Solid");
    }
    else if (LandingScorePercent >= 62.0f)
    {
        LandingRating = TEXT("Firm");
    }
    else if (LandingScorePercent >= 45.0f)
    {
        LandingRating = TEXT("Rough");
    }
    else
    {
        LandingRating = TEXT("Unsafe");
    }
}

void UFlightSimMovementComponent::DrawPracticeStripDebug(const UWorld* World) const
{
    if (!World || !bShowPracticeStripDebug || !bUseVirtualPracticeStrip)
    {
        return;
    }

    const FVector StripForward = GetPracticeStripForwardVector();
    const FVector StripRight = GetPracticeStripRightVector();
    const float HalfLengthCm = PracticeStripLengthCm * 0.5f;
    const float HalfWidthCm = PracticeStripWidthCm * 0.5f;
    const FVector StripLift(0.0f, 0.0f, 6.0f);

    const FVector FrontLeft = PracticeStripCenter + (StripForward * HalfLengthCm) - (StripRight * HalfWidthCm) + StripLift;
    const FVector FrontRight = PracticeStripCenter + (StripForward * HalfLengthCm) + (StripRight * HalfWidthCm) + StripLift;
    const FVector RearLeft = PracticeStripCenter - (StripForward * HalfLengthCm) - (StripRight * HalfWidthCm) + StripLift;
    const FVector RearRight = PracticeStripCenter - (StripForward * HalfLengthCm) + (StripRight * HalfWidthCm) + StripLift;
    const FVector CenterStart = PracticeStripCenter - (StripForward * HalfLengthCm) + StripLift;
    const FVector CenterEnd = PracticeStripCenter + (StripForward * HalfLengthCm) + StripLift;

    DrawDebugLine(World, FrontLeft, FrontRight, FColor(230, 198, 72), false, -1.0f, 0, 8.0f);
    DrawDebugLine(World, FrontRight, RearRight, FColor(230, 198, 72), false, -1.0f, 0, 8.0f);
    DrawDebugLine(World, RearRight, RearLeft, FColor(230, 198, 72), false, -1.0f, 0, 8.0f);
    DrawDebugLine(World, RearLeft, FrontLeft, FColor(230, 198, 72), false, -1.0f, 0, 8.0f);
    DrawDebugLine(World, CenterStart, CenterEnd, FColor(113, 186, 255), false, -1.0f, 0, 3.0f);
}
