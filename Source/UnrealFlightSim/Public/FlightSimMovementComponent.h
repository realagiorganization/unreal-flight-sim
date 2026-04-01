#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FlightSimMovementComponent.generated.h"

class AActor;

UCLASS(ClassGroup=(Flight), meta=(BlueprintSpawnableComponent))
class UNREALFLIGHTSIM_API UFlightSimMovementComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UFlightSimMovementComponent();

    virtual void BeginPlay() override;
    virtual void TickComponent(
        float DeltaTime,
        ELevelTick TickType,
        FActorComponentTickFunction* ThisTickFunction
    ) override;

    void SetThrottleInput(float Value);
    void SetPitchInput(float Value);
    void SetRollInput(float Value);
    void SetYawInput(float Value);
    void ResetFlightState();

    UFUNCTION(BlueprintPure, Category="Flight")
    float GetAirspeedKnots() const;

    UFUNCTION(BlueprintPure, Category="Flight")
    float GetThrottlePercent() const;

    UFUNCTION(BlueprintPure, Category="Flight|Telemetry")
    float GetGroundSpeedKnots() const;

    UFUNCTION(BlueprintPure, Category="Flight|Telemetry")
    float GetAltitudeAboveGroundMeters() const;

    UFUNCTION(BlueprintPure, Category="Flight|Telemetry")
    float GetTerrainProximityAlpha() const;

    UFUNCTION(BlueprintPure, Category="Flight|Telemetry")
    float GetStallSeverity() const;

    UFUNCTION(BlueprintPure, Category="Flight|Weather")
    FVector GetWindVectorWorldMps() const;

private:
    UPROPERTY(EditAnywhere, Category="Flight|Speed")
    float MinSpeedMps = 70.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Speed")
    float MaxSpeedMps = 190.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Speed")
    float SpeedResponse = 0.8f;

    UPROPERTY(EditAnywhere, Category="Flight|Speed")
    float ThrottleResponse = 0.35f;

    UPROPERTY(EditAnywhere, Category="Flight|Handling")
    float MaxPitchRateDegrees = 38.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Handling")
    float MaxYawRateDegrees = 22.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Handling")
    float MaxBankAngleDegrees = 55.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Handling")
    float BankResponse = 2.1f;

    UPROPERTY(EditAnywhere, Category="Flight|Handling")
    float RollVisualResponse = 5.5f;

    UPROPERTY(EditAnywhere, Category="Flight|Handling")
    float BankTurnRateDegrees = 52.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Handling")
    float CruiseLiftMps = 6.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Handling")
    float MinimumAltitudeCm = 12000.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Handling")
    float StallSpeedMps = 84.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Handling")
    float StallAuthorityLoss = 0.65f;

    UPROPERTY(EditAnywhere, Category="Flight|Handling")
    float StallSinkRateMps = 16.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Handling")
    float GroundEffectHeightCm = 3200.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Handling")
    float GroundEffectLiftMps = 3.4f;

    UPROPERTY(EditAnywhere, Category="Flight|Terrain")
    float TerrainProbeLengthCm = 200000.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Terrain")
    float TerrainAwarenessHeightCm = 30000.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Weather")
    FVector BaseWindWorldMps = FVector(6.0f, 1.6f, 0.0f);

    UPROPERTY(EditAnywhere, Category="Flight|Weather")
    float GustAmplitudeMps = 5.5f;

    UPROPERTY(EditAnywhere, Category="Flight|Weather")
    float GustVerticalAmplitudeMps = 1.5f;

    UPROPERTY(EditAnywhere, Category="Flight|Weather")
    float GustFrequencyHz = 0.18f;

    UPROPERTY(EditAnywhere, Category="Flight|Weather")
    float WeathercockYawRateDegrees = 8.0f;

    float AirspeedMps = 95.0f;
    float ThrottlePercent = 0.55f;
    float ThrottleInput = 0.0f;
    float PitchInput = 0.0f;
    float RollInput = 0.0f;
    float YawInput = 0.0f;
    float BankAngleDegrees = 0.0f;
    float GroundSpeedMps = 95.0f;
    float AltitudeAboveGroundCm = 30000.0f;
    float TerrainProximity = 0.0f;
    float StallSeverityValue = 0.0f;
    FVector CurrentWindWorldMps = FVector::ZeroVector;

    void UpdateEnvironment(const AActor* Owner);
};
