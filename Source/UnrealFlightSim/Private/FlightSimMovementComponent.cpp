#include "FlightSimMovementComponent.h"

#include "GameFramework/Actor.h"

UFlightSimMovementComponent::UFlightSimMovementComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
}

void UFlightSimMovementComponent::BeginPlay()
{
    Super::BeginPlay();
    AirspeedMps = FMath::Clamp(AirspeedMps, MinSpeedMps, MaxSpeedMps);
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

    ThrottlePercent = FMath::Clamp(ThrottlePercent + (ThrottleInput * ThrottleResponse * DeltaTime), 0.0f, 1.0f);

    const float DesiredSpeed = FMath::Lerp(MinSpeedMps, MaxSpeedMps, ThrottlePercent);
    AirspeedMps = FMath::FInterpTo(AirspeedMps, DesiredSpeed, DeltaTime, SpeedResponse);

    const float SafeMaxBank = FMath::Max(MaxBankAngleDegrees, 1.0f);
    const float DesiredBank = RollInput * MaxBankAngleDegrees;
    BankAngleDegrees = FMath::FInterpTo(BankAngleDegrees, DesiredBank, DeltaTime, BankResponse);

    const FRotator CurrentRotation = Owner->GetActorRotation();
    const float NewPitch = FMath::Clamp(CurrentRotation.Pitch + (PitchInput * MaxPitchRateDegrees * DeltaTime), -35.0f, 40.0f);
    const float NewYaw = CurrentRotation.Yaw
        + ((YawInput * MaxYawRateDegrees) + ((BankAngleDegrees / SafeMaxBank) * BankTurnRateDegrees)) * DeltaTime;
    const float NewRoll = FMath::FInterpTo(CurrentRotation.Roll, -BankAngleDegrees, DeltaTime, RollVisualResponse);
    const FRotator NewRotation(NewPitch, NewYaw, NewRoll);

    Owner->SetActorRotation(NewRotation);

    FVector DeltaLocation = Owner->GetActorForwardVector() * (AirspeedMps * DeltaTime * 100.0f);
    DeltaLocation.Z += (PitchInput * 1800.0f + ThrottlePercent * CruiseLiftMps * 100.0f) * DeltaTime;

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
    ThrottlePercent = 0.55f;
    ThrottleInput = 0.0f;
    PitchInput = 0.0f;
    RollInput = 0.0f;
    YawInput = 0.0f;
    BankAngleDegrees = 0.0f;
}

float UFlightSimMovementComponent::GetAirspeedKnots() const
{
    return AirspeedMps * 1.94384f;
}

float UFlightSimMovementComponent::GetThrottlePercent() const
{
    return ThrottlePercent;
}
