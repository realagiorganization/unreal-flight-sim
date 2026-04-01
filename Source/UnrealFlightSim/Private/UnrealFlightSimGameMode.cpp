#include "UnrealFlightSimGameMode.h"

#include "FlightSimPawn.h"

AUnrealFlightSimGameMode::AUnrealFlightSimGameMode()
{
    DefaultPawnClass = AFlightSimPawn::StaticClass();
}
