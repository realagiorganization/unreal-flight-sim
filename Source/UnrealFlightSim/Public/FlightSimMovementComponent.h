#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FlightSimMovementComponent.generated.h"

class AActor;
class UWorld;

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
    void SetPayloadWeightKg(float Value);
    void ResetFlightState();

    UFUNCTION(BlueprintPure, Category="Flight")
    float GetAirspeedKnots() const;

    UFUNCTION(BlueprintPure, Category="Flight")
    float GetThrottlePercent() const;

    UFUNCTION(BlueprintPure, Category="Flight|Load")
    float GetPayloadWeightKg() const;

    UFUNCTION(BlueprintPure, Category="Flight|Load")
    float GetPayloadRatio() const;

    UFUNCTION(BlueprintPure, Category="Flight|Telemetry")
    float GetGroundSpeedKnots() const;

    UFUNCTION(BlueprintPure, Category="Flight|Telemetry")
    float GetAltitudeAboveGroundMeters() const;

    UFUNCTION(BlueprintPure, Category="Flight|Telemetry")
    float GetTerrainProximityAlpha() const;

    UFUNCTION(BlueprintPure, Category="Flight|Telemetry")
    float GetVerticalSpeedMps() const;

    UFUNCTION(BlueprintPure, Category="Flight|Telemetry")
    float GetStallSeverity() const;

    UFUNCTION(BlueprintPure, Category="Flight|Weather")
    FVector GetWindVectorWorldMps() const;

    UFUNCTION(BlueprintPure, Category="Flight|Landing")
    bool IsGrounded() const;

    UFUNCTION(BlueprintPure, Category="Flight|Landing")
    bool HasLandingEvaluation() const;

    UFUNCTION(BlueprintPure, Category="Flight|Landing")
    float GetLandingScorePercent() const;

    UFUNCTION(BlueprintPure, Category="Flight|Landing")
    FString GetLandingRating() const;

    UFUNCTION(BlueprintPure, Category="Flight|Landing")
    float GetTouchdownSpeedKnots() const;

    UFUNCTION(BlueprintPure, Category="Flight|Landing")
    float GetTouchdownSinkRateMps() const;

    UFUNCTION(BlueprintPure, Category="Flight|Landing")
    float GetLandingRollDistanceMeters() const;

    UFUNCTION(BlueprintPure, Category="Flight|Landing")
    float GetRunwayRemainingMeters() const;

    UFUNCTION(BlueprintPure, Category="Flight|Landing")
    float GetRunwayAlignmentErrorDegrees() const;

    UFUNCTION(BlueprintPure, Category="Flight|Landing")
    float GetRunwayCenterDeviationMeters() const;

    UFUNCTION(BlueprintPure, Category="Flight|Landing")
    bool WasLandingCrash() const;

    UFUNCTION(BlueprintPure, Category="Flight|Landing")
    bool WasLandingOverrun() const;

private:
    UPROPERTY(EditAnywhere, Category="Flight|Speed")
    float MinSpeedMps = 24.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Speed")
    float MaxSpeedMps = 62.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Speed")
    float SpeedResponse = 1.15f;

    UPROPERTY(EditAnywhere, Category="Flight|Speed")
    float ThrottleResponse = 0.42f;

    UPROPERTY(EditAnywhere, Category="Flight|Handling")
    float MaxPitchRateDegrees = 32.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Handling")
    float MaxYawRateDegrees = 18.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Handling")
    float MaxBankAngleDegrees = 48.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Handling")
    float BankResponse = 2.5f;

    UPROPERTY(EditAnywhere, Category="Flight|Handling")
    float RollVisualResponse = 4.8f;

    UPROPERTY(EditAnywhere, Category="Flight|Handling")
    float BankTurnRateDegrees = 34.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Handling")
    float CruiseLiftMps = 2.8f;

    UPROPERTY(EditAnywhere, Category="Flight|Handling")
    float TouchdownClearanceCm = 110.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Handling")
    float StallSpeedMps = 23.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Handling")
    float StallAuthorityLoss = 0.65f;

    UPROPERTY(EditAnywhere, Category="Flight|Handling")
    float StallSinkRateMps = 8.5f;

    UPROPERTY(EditAnywhere, Category="Flight|Handling")
    float GroundEffectHeightCm = 1800.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Handling")
    float GroundEffectLiftMps = 2.2f;

    UPROPERTY(EditAnywhere, Category="Flight|Terrain")
    float TerrainProbeLengthCm = 200000.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Terrain")
    float TerrainAwarenessHeightCm = 12000.0f;

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

    UPROPERTY(EditAnywhere, Category="Flight|Load")
    float PayloadWeightKg = 210.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Load")
    float MaxPayloadWeightKg = 420.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Load")
    float PayloadStallPenalty = 0.22f;

    UPROPERTY(EditAnywhere, Category="Flight|Load")
    float PayloadLiftPenalty = 0.38f;

    UPROPERTY(EditAnywhere, Category="Flight|Load")
    float PayloadAuthorityPenalty = 0.18f;

    UPROPERTY(EditAnywhere, Category="Flight|Load")
    float PayloadGroundRollPenalty = 0.32f;

    UPROPERTY(EditAnywhere, Category="Flight|Landing")
    bool bUseVirtualPracticeStrip = true;

    UPROPERTY(EditAnywhere, Category="Flight|Landing")
    bool bShowPracticeStripDebug = true;

    UPROPERTY(EditAnywhere, Category="Flight|Landing")
    FVector PracticeStripCenter = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, Category="Flight|Landing")
    float PracticeStripHeadingDegrees = 0.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Landing")
    float PracticeStripLengthCm = 18000.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Landing")
    float PracticeStripWidthCm = 2400.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Landing")
    float GroundRollDecelerationMps2 = 5.5f;

    UPROPERTY(EditAnywhere, Category="Flight|Landing")
    float CrashGroundDecelerationMps2 = 11.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Landing")
    float GroundStopSpeedMps = 1.5f;

    UPROPERTY(EditAnywhere, Category="Flight|Landing")
    float MaxSafeTouchdownSpeedMps = 28.0f;

    UPROPERTY(EditAnywhere, Category="Flight|Landing")
    float MaxSafeTouchdownSinkRateMps = 2.2f;

    UPROPERTY(EditAnywhere, Category="Flight|Landing")
    float MaxSafeTouchdownBankDegrees = 9.0f;

    float AirspeedMps = 30.0f;
    float ThrottlePercent = 0.55f;
    float ThrottleInput = 0.0f;
    float PitchInput = 0.0f;
    float RollInput = 0.0f;
    float YawInput = 0.0f;
    float BankAngleDegrees = 0.0f;
    float GroundSpeedMps = 30.0f;
    float VerticalSpeedMps = 0.0f;
    float AltitudeAboveGroundCm = 30000.0f;
    float TerrainProximity = 0.0f;
    float StallSeverityValue = 0.0f;
    FVector CurrentWindWorldMps = FVector::ZeroVector;
    bool bHasGroundReference = false;
    bool bIsGrounded = false;
    bool bLandingEvaluationReady = false;
    bool bLandingWasCrash = false;
    bool bLandingWasOverrun = false;
    float GroundHeightCm = 0.0f;
    float StripTravelDirection = 1.0f;
    float LandingScorePercent = 0.0f;
    FString LandingRating = TEXT("In Flight");
    float TouchdownSpeedMps = 0.0f;
    float TouchdownSinkRateMps = 0.0f;
    float LandingRollDistanceCm = 0.0f;
    float RunwayRemainingCm = 0.0f;
    float RunwayAlignmentErrorDegrees = 0.0f;
    float RunwayCenterDeviationCm = 0.0f;

    void UpdateEnvironment(const AActor* Owner);
    FVector GetPracticeStripForwardVector() const;
    FVector GetPracticeStripRightVector() const;
    bool GetStripFrameMetrics(
        const FVector& WorldLocation,
        float HeadingSign,
        float& OutLongitudinalCm,
        float& OutLateralCm,
        float& OutRunwayRemainingCm
    ) const;
    void FinalizeTouchdown(AActor* Owner, const FVector& TouchdownLocation, const FRotator& TouchdownRotation);
    void UpdateGroundRoll(AActor* Owner, float DeltaTime);
    void UpdateLandingEvaluation();
    void DrawPracticeStripDebug(const UWorld* World) const;
};
