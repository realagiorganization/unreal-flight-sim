#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "FlightSimHUD.generated.h"

UCLASS()
class UNREALFLIGHTSIM_API AFlightSimHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void DrawHUD() override;

    void ToggleOverlay();

private:
    bool bShowOverlay = true;
};
