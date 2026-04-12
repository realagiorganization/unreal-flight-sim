#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UnrealFlightSimGameMode.generated.h"

class AFlightSimPawn;
class UFlightSimMovementComponent;

struct FFlightCargoMissionDefinition
{
    FString Name;
    FString Destination;
    float PayloadKg = 0.0f;
    float MinimumLandingScore = 0.0f;
    float PayoutDollars = 0.0f;
    float MaxCenterDeviationMeters = 0.0f;
    FString DifficultyLabel;
    FString BriefingNote;
};

UCLASS()
class UNREALFLIGHTSIM_API AUnrealFlightSimGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AUnrealFlightSimGameMode();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSeconds) override;

    void ResetCargoMission();

    UFUNCTION(BlueprintPure, Category="Mission|Cargo")
    FString GetMissionName() const;

    UFUNCTION(BlueprintPure, Category="Mission|Cargo")
    FString GetMissionBriefingText() const;

    UFUNCTION(BlueprintPure, Category="Mission|Cargo")
    FString GetMissionStatusText() const;

    UFUNCTION(BlueprintPure, Category="Mission|Cargo")
    FString GetMissionDebriefText() const;

    UFUNCTION(BlueprintPure, Category="Mission|Cargo")
    float GetMissionPayloadKg() const;

    UFUNCTION(BlueprintPure, Category="Mission|Cargo")
    float GetMissionMinimumLandingScore() const;

    UFUNCTION(BlueprintPure, Category="Mission|Cargo")
    float GetMissionPayoutDollars() const;

    UFUNCTION(BlueprintPure, Category="Mission|Cargo")
    bool IsMissionResolved() const;

    UFUNCTION(BlueprintPure, Category="Mission|Cargo")
    bool WasMissionSuccessful() const;

    UFUNCTION(BlueprintPure, Category="Mission|Campaign")
    int32 GetCampaignBalanceDollars() const;

    UFUNCTION(BlueprintPure, Category="Mission|Campaign")
    int32 GetCompletedContracts() const;

    UFUNCTION(BlueprintPure, Category="Mission|Campaign")
    int32 GetFailedContracts() const;

    UFUNCTION(BlueprintPure, Category="Mission|Campaign")
    int32 GetSuccessStreak() const;

    UFUNCTION(BlueprintPure, Category="Mission|Campaign")
    int32 GetCurrentMissionNumber() const;

    UFUNCTION(BlueprintPure, Category="Mission|Campaign")
    int32 GetMissionCount() const;

    UFUNCTION(BlueprintPure, Category="Mission|Campaign")
    bool WillAdvanceMissionOnReset() const;

private:
    UPROPERTY(EditAnywhere, Category="Mission|Campaign")
    int32 StartingBalanceDollars = 2500;

    UPROPERTY(EditAnywhere, Category="Mission|Campaign")
    int32 FailurePenaltyDollars = 350;

    bool bMissionResolved = false;
    bool bMissionSuccessful = false;
    bool bAdvanceMissionOnReset = false;
    int32 ActiveMissionIndex = 0;
    int32 CampaignBalanceDollars = 0;
    int32 CompletedContracts = 0;
    int32 FailedContracts = 0;
    int32 SuccessStreak = 0;
    FString MissionStatusText = TEXT("Mission not started");
    FString MissionDebriefText;
    TArray<FFlightCargoMissionDefinition> MissionCatalog;

    AFlightSimPawn* GetFlightPawn() const;
    UFlightSimMovementComponent* GetFlightMovement() const;
    const FFlightCargoMissionDefinition& GetActiveMission() const;
    void ApplyMissionPayload();
    void ResolveMission(bool bSuccess, const FString& Debrief);
    void InitializeMissionCatalog();
};
