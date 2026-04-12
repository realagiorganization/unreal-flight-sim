#include "FlightSimHUD.h"

#include "FlightSimMovementComponent.h"
#include "FlightSimPawn.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "UnrealFlightSimGameMode.h"

void AFlightSimHUD::DrawHUD()
{
    Super::DrawHUD();

    if (!bShowOverlay || !Canvas)
    {
        return;
    }

    const APlayerController* PlayerController = GetOwningPlayerController();
    const AFlightSimPawn* FlightPawn = PlayerController ? Cast<AFlightSimPawn>(PlayerController->GetPawn()) : nullptr;
    const UFlightSimMovementComponent* FlightMovement = FlightPawn ? FlightPawn->GetFlightMovement() : nullptr;
    const AUnrealFlightSimGameMode* FlightGameMode =
        GetWorld() ? Cast<AUnrealFlightSimGameMode>(GetWorld()->GetAuthGameMode()) : nullptr;

    if (!FlightMovement)
    {
        DrawText(TEXT("Flight HUD unavailable"), FLinearColor::Red, 32.0f, 32.0f, nullptr, 1.2f, false);
        return;
    }

    const float AirspeedKnots = FlightMovement->GetAirspeedKnots();
    const float GroundSpeedKnots = FlightMovement->GetGroundSpeedKnots();
    const float VerticalSpeedFpm = FlightMovement->GetVerticalSpeedMps() * 196.8504f;
    const float WindSpeedKnots = FlightMovement->GetWindVectorWorldMps().Size() * 1.94384f;
    const float StallSeverityPercent = FlightMovement->GetStallSeverity() * 100.0f;
    const float PayloadKg = FlightMovement->GetPayloadWeightKg();

    DrawText(TEXT("BUSH FLYING DEBUG"), FLinearColor(0.95f, 0.87f, 0.43f), 32.0f, 28.0f, nullptr, 1.2f, false);
    if (FlightGameMode)
    {
        const bool bMissionResolved = FlightGameMode->IsMissionResolved();
        const FLinearColor MissionColor = !bMissionResolved
            ? FLinearColor(0.91f, 0.93f, 0.98f)
            : (FlightGameMode->WasMissionSuccessful()
                ? FLinearColor(0.47f, 0.88f, 0.45f)
                : FLinearColor(1.0f, 0.40f, 0.34f));

        DrawText(
            FString::Printf(
                TEXT("Campaign $%d   Completed %d   Failed %d   Streak %d"),
                FlightGameMode->GetCampaignBalanceDollars(),
                FlightGameMode->GetCompletedContracts(),
                FlightGameMode->GetFailedContracts(),
                FlightGameMode->GetSuccessStreak()
            ),
            FLinearColor::White,
            32.0f,
            56.0f,
            nullptr,
            1.0f,
            false
        );
        DrawText(
            FString::Printf(
                TEXT("Contract %d/%d   %s"),
                FlightGameMode->GetCurrentMissionNumber(),
                FlightGameMode->GetMissionCount(),
                *FlightGameMode->GetMissionName()
            ),
            FLinearColor(0.84f, 0.89f, 1.0f),
            32.0f,
            80.0f,
            nullptr,
            0.95f,
            false
        );
        DrawText(
            FlightGameMode->GetMissionBriefingText(),
            FLinearColor(0.84f, 0.89f, 1.0f),
            32.0f,
            104.0f,
            nullptr,
            0.95f,
            false
        );
        DrawText(
            FString::Printf(TEXT("Status %s"), *FlightGameMode->GetMissionStatusText()),
            MissionColor,
            32.0f,
            128.0f,
            nullptr,
            0.95f,
            false
        );

        if (bMissionResolved)
        {
            DrawText(
                FlightGameMode->GetMissionDebriefText(),
                MissionColor,
                32.0f,
                152.0f,
                nullptr,
                0.92f,
                false
            );
        }
    }

    DrawText(
        FString::Printf(TEXT("IAS %.0f kt   GS %.0f kt   VSI %.0f fpm"), AirspeedKnots, GroundSpeedKnots, VerticalSpeedFpm),
        FLinearColor::White,
        32.0f,
        188.0f,
        nullptr,
        1.0f,
        false
    );
    DrawText(
        FString::Printf(
            TEXT("Throttle %.0f%%   Payload %.0f kg   AGL %.1f m   Stall %.0f%%   Wind %.0f kt"),
            FlightMovement->GetThrottlePercent() * 100.0f,
            PayloadKg,
            FlightMovement->GetAltitudeAboveGroundMeters(),
            StallSeverityPercent,
            WindSpeedKnots
        ),
        FLinearColor::White,
        32.0f,
        212.0f,
        nullptr,
        1.0f,
        false
    );
    DrawText(
        FString::Printf(
            TEXT("Runway remain %.0f m   Center dev %.1f m   Align err %.0f deg"),
            FlightMovement->GetRunwayRemainingMeters(),
            FlightMovement->GetRunwayCenterDeviationMeters(),
            FlightMovement->GetRunwayAlignmentErrorDegrees()
        ),
        FLinearColor(0.72f, 0.84f, 1.0f),
        32.0f,
        236.0f,
        nullptr,
        1.0f,
        false
    );

    if (FlightMovement->HasLandingEvaluation())
    {
        const float LandingScore = FlightMovement->GetLandingScorePercent();
        const FLinearColor ResultColor = LandingScore >= 78.0f
            ? FLinearColor(0.47f, 0.88f, 0.45f)
            : (LandingScore >= 50.0f ? FLinearColor(0.96f, 0.77f, 0.30f) : FLinearColor(1.0f, 0.40f, 0.34f));

        DrawText(
            FString::Printf(TEXT("Landing %s   Score %.0f"), *FlightMovement->GetLandingRating(), LandingScore),
            ResultColor,
            32.0f,
            272.0f,
            nullptr,
            1.15f,
            false
        );
        DrawText(
            FString::Printf(
                TEXT("Touchdown %.0f kt   Sink %.1f m/s   Rollout %.0f m"),
                FlightMovement->GetTouchdownSpeedKnots(),
                FlightMovement->GetTouchdownSinkRateMps(),
                FlightMovement->GetLandingRollDistanceMeters()
            ),
            ResultColor,
            32.0f,
            296.0f,
            nullptr,
            1.0f,
            false
        );
        DrawText(
            FlightGameMode && FlightGameMode->WillAdvanceMissionOnReset()
                ? TEXT("Press R to load the next contract")
                : TEXT("Press R to retry the cargo run"),
            FLinearColor::White,
            32.0f,
            320.0f,
            nullptr,
            0.95f,
            false
        );
    }
    else
    {
        DrawText(TEXT("Follow the yellow debug strip and press R to reset"), FLinearColor::White, 32.0f, 272.0f, nullptr, 0.95f, false);
    }

    DrawText(TEXT("H toggles this HUD"), FLinearColor(0.82f, 0.82f, 0.82f), 32.0f, 356.0f, nullptr, 0.9f, false);
}

void AFlightSimHUD::ToggleOverlay()
{
    bShowOverlay = !bShowOverlay;
}
