#include "UnrealFlightSimGameMode.h"

#include "FlightSimHUD.h"
#include "FlightSimMovementComponent.h"
#include "FlightSimPawn.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

AUnrealFlightSimGameMode::AUnrealFlightSimGameMode()
{
    PrimaryActorTick.bCanEverTick = true;
    DefaultPawnClass = AFlightSimPawn::StaticClass();
    HUDClass = AFlightSimHUD::StaticClass();
    InitializeMissionCatalog();
}

void AUnrealFlightSimGameMode::BeginPlay()
{
    Super::BeginPlay();

    CampaignBalanceDollars = StartingBalanceDollars;
    CompletedContracts = 0;
    FailedContracts = 0;
    SuccessStreak = 0;
    ActiveMissionIndex = 0;
    bAdvanceMissionOnReset = false;
    ResetCargoMission();
}

void AUnrealFlightSimGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);
    (void)DeltaSeconds;

    UFlightSimMovementComponent* FlightMovement = GetFlightMovement();
    if (!FlightMovement)
    {
        MissionStatusText = TEXT("Waiting for aircraft");
        return;
    }

    if (bMissionResolved)
    {
        return;
    }

    const FFlightCargoMissionDefinition& ActiveMission = GetActiveMission();

    if (!FlightMovement->HasLandingEvaluation())
    {
        MissionStatusText = FString::Printf(
            TEXT("Deliver %.0f kg to %s. Need %.0f+ score and %.1f m centerline discipline."),
            ActiveMission.PayloadKg,
            *ActiveMission.Destination,
            ActiveMission.MinimumLandingScore,
            ActiveMission.MaxCenterDeviationMeters
        );
        return;
    }

    if (FlightMovement->WasLandingCrash())
    {
        ResolveMission(false, TEXT("Cargo destroyed on impact."));
        return;
    }

    if (FlightMovement->WasLandingOverrun())
    {
        ResolveMission(false, TEXT("Strip overrun. Cargo not delivered."));
        return;
    }

    if (FlightMovement->GetGroundSpeedKnots() > 2.0f)
    {
        MissionStatusText = TEXT("Hold the rollout straight and stop on the strip.");
        return;
    }

    const bool bDeliveredCleanly = FlightMovement->GetLandingScorePercent() >= ActiveMission.MinimumLandingScore
        && FlightMovement->GetRunwayCenterDeviationMeters() <= ActiveMission.MaxCenterDeviationMeters
        && !FlightMovement->WasLandingCrash()
        && !FlightMovement->WasLandingOverrun();

    if (bDeliveredCleanly)
    {
        ResolveMission(
            true,
            FString::Printf(
                TEXT("Cargo delivered. Score %.0f. Contract paid $%.0f."),
                FlightMovement->GetLandingScorePercent(),
                ActiveMission.PayoutDollars
            )
        );
    }
    else
    {
        ResolveMission(
            false,
            FString::Printf(
                TEXT("Landing missed contract minimums. Need %.0f+ score and %.1f m max center deviation."),
                ActiveMission.MinimumLandingScore,
                ActiveMission.MaxCenterDeviationMeters
            )
        );
    }
}

void AUnrealFlightSimGameMode::ResetCargoMission()
{
    if (MissionCatalog.Num() == 0)
    {
        InitializeMissionCatalog();
    }

    if (bAdvanceMissionOnReset && MissionCatalog.Num() > 0)
    {
        ActiveMissionIndex = (ActiveMissionIndex + 1) % MissionCatalog.Num();
        bAdvanceMissionOnReset = false;
    }

    const FFlightCargoMissionDefinition& ActiveMission = GetActiveMission();

    bMissionResolved = false;
    bMissionSuccessful = false;
    MissionStatusText = FString::Printf(
        TEXT("Deliver %.0f kg to %s. Need %.0f+ score and %.1f m centerline discipline."),
        ActiveMission.PayloadKg,
        *ActiveMission.Destination,
        ActiveMission.MinimumLandingScore,
        ActiveMission.MaxCenterDeviationMeters
    );
    MissionDebriefText = TEXT("");

    ApplyMissionPayload();
}

FString AUnrealFlightSimGameMode::GetMissionName() const
{
    return GetActiveMission().Name;
}

FString AUnrealFlightSimGameMode::GetMissionBriefingText() const
{
    const FFlightCargoMissionDefinition& ActiveMission = GetActiveMission();
    return FString::Printf(
        TEXT("%s | %.0f kg | %.0f+ score | $%.0f | %s"),
        *ActiveMission.DifficultyLabel,
        ActiveMission.PayloadKg,
        ActiveMission.MinimumLandingScore,
        ActiveMission.PayoutDollars,
        *ActiveMission.BriefingNote
    );
}

FString AUnrealFlightSimGameMode::GetMissionStatusText() const
{
    return MissionStatusText;
}

FString AUnrealFlightSimGameMode::GetMissionDebriefText() const
{
    return MissionDebriefText;
}

float AUnrealFlightSimGameMode::GetMissionPayloadKg() const
{
    return GetActiveMission().PayloadKg;
}

float AUnrealFlightSimGameMode::GetMissionMinimumLandingScore() const
{
    return GetActiveMission().MinimumLandingScore;
}

float AUnrealFlightSimGameMode::GetMissionPayoutDollars() const
{
    return GetActiveMission().PayoutDollars;
}

bool AUnrealFlightSimGameMode::IsMissionResolved() const
{
    return bMissionResolved;
}

bool AUnrealFlightSimGameMode::WasMissionSuccessful() const
{
    return bMissionSuccessful;
}

int32 AUnrealFlightSimGameMode::GetCampaignBalanceDollars() const
{
    return CampaignBalanceDollars;
}

int32 AUnrealFlightSimGameMode::GetCompletedContracts() const
{
    return CompletedContracts;
}

int32 AUnrealFlightSimGameMode::GetFailedContracts() const
{
    return FailedContracts;
}

int32 AUnrealFlightSimGameMode::GetSuccessStreak() const
{
    return SuccessStreak;
}

int32 AUnrealFlightSimGameMode::GetCurrentMissionNumber() const
{
    return MissionCatalog.Num() == 0 ? 0 : ActiveMissionIndex + 1;
}

int32 AUnrealFlightSimGameMode::GetMissionCount() const
{
    return MissionCatalog.Num();
}

bool AUnrealFlightSimGameMode::WillAdvanceMissionOnReset() const
{
    return bAdvanceMissionOnReset;
}

AFlightSimPawn* AUnrealFlightSimGameMode::GetFlightPawn() const
{
    if (UWorld* World = GetWorld())
    {
        if (APlayerController* PlayerController = World->GetFirstPlayerController())
        {
            return Cast<AFlightSimPawn>(PlayerController->GetPawn());
        }
    }

    return nullptr;
}

UFlightSimMovementComponent* AUnrealFlightSimGameMode::GetFlightMovement() const
{
    AFlightSimPawn* FlightPawn = GetFlightPawn();
    return FlightPawn ? FlightPawn->GetFlightMovement() : nullptr;
}

const FFlightCargoMissionDefinition& AUnrealFlightSimGameMode::GetActiveMission() const
{
    check(MissionCatalog.Num() > 0);
    return MissionCatalog[FMath::Clamp(ActiveMissionIndex, 0, MissionCatalog.Num() - 1)];
}

void AUnrealFlightSimGameMode::ApplyMissionPayload()
{
    if (UFlightSimMovementComponent* FlightMovement = GetFlightMovement())
    {
        FlightMovement->SetPayloadWeightKg(GetActiveMission().PayloadKg);
    }
}

void AUnrealFlightSimGameMode::ResolveMission(bool bSuccess, const FString& Debrief)
{
    bMissionResolved = true;
    bMissionSuccessful = bSuccess;

    if (bSuccess)
    {
        CompletedContracts++;
        SuccessStreak++;
        CampaignBalanceDollars += FMath::RoundToInt(GetActiveMission().PayoutDollars);
        bAdvanceMissionOnReset = true;

        const int32 NextMissionNumber = MissionCatalog.Num() == 0 ? 0 : ((ActiveMissionIndex + 1) % MissionCatalog.Num()) + 1;
        MissionStatusText = FString::Printf(TEXT("Contract complete. Press R to load contract %d."), NextMissionNumber);
        MissionDebriefText = FString::Printf(
            TEXT("%s Balance $%d. Streak %d."),
            *Debrief,
            CampaignBalanceDollars,
            SuccessStreak
        );
        return;
    }

    FailedContracts++;
    SuccessStreak = 0;
    CampaignBalanceDollars = FMath::Max(0, CampaignBalanceDollars - FailurePenaltyDollars);
    bAdvanceMissionOnReset = false;
    MissionStatusText = TEXT("Contract failed. Press R to retry the same job.");
    MissionDebriefText = FString::Printf(
        TEXT("%s Repairs and penalties cost $%d. Balance $%d."),
        *Debrief,
        FailurePenaltyDollars,
        CampaignBalanceDollars
    );
}

void AUnrealFlightSimGameMode::InitializeMissionCatalog()
{
    if (MissionCatalog.Num() > 0)
    {
        return;
    }

    MissionCatalog = {
        {
            TEXT("Valley Mail Run"),
            TEXT("the backcountry strip"),
            180.0f,
            70.0f,
            1450.0f,
            5.5f,
            TEXT("Easy"),
            TEXT("Mail sacks and food tins for a short valley hop.")
        },
        {
            TEXT("Ridge Supply Hop"),
            TEXT("the ridge shelf strip"),
            260.0f,
            76.0f,
            1950.0f,
            4.5f,
            TEXT("Medium"),
            TEXT("Bulk supplies with a heavier approach and longer rollout.")
        },
        {
            TEXT("River Medical Drop"),
            TEXT("the river gravel bar"),
            140.0f,
            82.0f,
            2350.0f,
            3.8f,
            TEXT("Hard"),
            TEXT("Light payload, tight touchdown window, no sloppy centerline drift.")
        },
        {
            TEXT("Storm Front Relief"),
            TEXT("the emergency relief strip"),
            320.0f,
            86.0f,
            3200.0f,
            3.2f,
            TEXT("Expert"),
            TEXT("Heavy cargo with almost no room for rollout mistakes.")
        }
    };
}
