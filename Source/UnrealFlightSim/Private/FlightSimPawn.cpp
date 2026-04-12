#include "FlightSimPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "FlightSimHUD.h"
#include "FlightSimMovementComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "UnrealFlightSimGameMode.h"
#include "UObject/ConstructorHelpers.h"

AFlightSimPawn::AFlightSimPawn()
{
    PrimaryActorTick.bCanEverTick = true;
    AutoPossessPlayer = EAutoReceiveInput::Player0;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
    RootComponent = SceneRoot;

    FuselageMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FuselageMesh"));
    FuselageMesh->SetupAttachment(SceneRoot);
    FuselageMesh->SetRelativeScale3D(FVector(2.6f, 0.55f, 0.55f));
    FuselageMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    WingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WingMesh"));
    WingMesh->SetupAttachment(SceneRoot);
    WingMesh->SetRelativeScale3D(FVector(0.35f, 4.8f, 0.08f));
    WingMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    TailPlaneMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TailPlaneMesh"));
    TailPlaneMesh->SetupAttachment(SceneRoot);
    TailPlaneMesh->SetRelativeLocation(FVector(-110.0f, 0.0f, 18.0f));
    TailPlaneMesh->SetRelativeScale3D(FVector(0.24f, 1.85f, 0.06f));
    TailPlaneMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    VerticalFinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VerticalFinMesh"));
    VerticalFinMesh->SetupAttachment(SceneRoot);
    VerticalFinMesh->SetRelativeLocation(FVector(-118.0f, 0.0f, 58.0f));
    VerticalFinMesh->SetRelativeScale3D(FVector(0.18f, 0.08f, 1.0f));
    VerticalFinMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(SceneRoot);
    SpringArm->TargetArmLength = CameraBaseArmLength;
    SpringArm->SocketOffset = FVector(0.0f, 0.0f, 35.0f);
    SpringArm->bEnableCameraLag = true;
    SpringArm->CameraLagSpeed = CameraBaseLagSpeed;
    SpringArm->bEnableCameraRotationLag = true;
    SpringArm->CameraRotationLagSpeed = 8.0f;
    SpringArm->bDoCollisionTest = false;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->FieldOfView = CameraBaseFieldOfView;

    FlightMovement = CreateDefaultSubobject<UFlightSimMovementComponent>(TEXT("FlightMovement"));

    static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (CubeMesh.Succeeded())
    {
        FuselageMesh->SetStaticMesh(CubeMesh.Object);
        WingMesh->SetStaticMesh(CubeMesh.Object);
        TailPlaneMesh->SetStaticMesh(CubeMesh.Object);
        VerticalFinMesh->SetStaticMesh(CubeMesh.Object);
    }
}

void AFlightSimPawn::BeginPlay()
{
    Super::BeginPlay();
    ResetFlight();
}

void AFlightSimPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (FMath::IsNearlyZero(CameraYawInputValue))
    {
        CameraYawDegrees = FMath::FInterpTo(CameraYawDegrees, 0.0f, DeltaTime, CameraYawReturnSpeed);
    }

    if (FMath::IsNearlyZero(CameraPitchInputValue))
    {
        CameraPitchDegrees = FMath::FInterpTo(
            CameraPitchDegrees,
            CameraDefaultPitchDegrees,
            DeltaTime,
            CameraPitchReturnSpeed
        );
    }

    ApplyCameraRig(DeltaTime);
}

void AFlightSimPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    check(PlayerInputComponent);

    PlayerInputComponent->BindAxis(TEXT("Throttle"), this, &AFlightSimPawn::HandleThrottle);
    PlayerInputComponent->BindAxis(TEXT("Pitch"), this, &AFlightSimPawn::HandlePitch);
    PlayerInputComponent->BindAxis(TEXT("Roll"), this, &AFlightSimPawn::HandleRoll);
    PlayerInputComponent->BindAxis(TEXT("Yaw"), this, &AFlightSimPawn::HandleYaw);
    PlayerInputComponent->BindAxis(TEXT("CameraYaw"), this, &AFlightSimPawn::HandleCameraYaw);
    PlayerInputComponent->BindAxis(TEXT("CameraPitch"), this, &AFlightSimPawn::HandleCameraPitch);
    PlayerInputComponent->BindAction(TEXT("ToggleDebugHud"), IE_Pressed, this, &AFlightSimPawn::ToggleDebugHud);
    PlayerInputComponent->BindAction(TEXT("ResetFlight"), IE_Pressed, this, &AFlightSimPawn::ResetFlight);
}

void AFlightSimPawn::ApplyCameraRig(float DeltaTime)
{
    const float GroundSpeedKnots = FlightMovement ? FlightMovement->GetGroundSpeedKnots() : 0.0f;
    const float TerrainProximity = FlightMovement ? FlightMovement->GetTerrainProximityAlpha() : 0.0f;
    const float StallSeverity = FlightMovement ? FlightMovement->GetStallSeverity() : 0.0f;
    const float SpeedAlpha = FMath::GetMappedRangeValueClamped(FVector2D(45.0f, 130.0f), FVector2D(0.0f, 1.0f), GroundSpeedKnots);
    const float TurnLookYawDegrees = FMath::Clamp(
        -GetActorRotation().Roll * CameraTurnLookFactor * (0.35f + (TerrainProximity * 0.65f)),
        -(CameraYawLimit * 0.35f),
        CameraYawLimit * 0.35f
    );

    const float TargetPitch = FMath::Clamp(
        CameraPitchDegrees + (TerrainProximity * CameraTerrainPitchBiasDegrees) + (StallSeverity * CameraStallPitchBiasDegrees),
        CameraPitchMin + CameraTerrainPitchBiasDegrees,
        CameraPitchMax + CameraStallPitchBiasDegrees
    );
    const float TargetYaw = FMath::Clamp(CameraYawDegrees + TurnLookYawDegrees, -CameraYawLimit, CameraYawLimit);
    const float TargetRoll = FMath::Clamp(GetActorRotation().Roll * CameraBankFollow, -10.0f, 10.0f);
    const float TargetArmLength = FMath::Clamp(
        CameraBaseArmLength + (SpeedAlpha * CameraSpeedArmExtension) - (TerrainProximity * CameraTerrainArmCompression)
            - (StallSeverity * CameraStallArmCompression),
        620.0f,
        1500.0f
    );
    const FVector TargetSocketOffset(
        0.0f,
        0.0f,
        35.0f + (TerrainProximity * CameraTerrainSocketLift) + (StallSeverity * CameraStallSocketLift)
    );
    const float TargetLagSpeed = FMath::Lerp(CameraBaseLagSpeed, CameraTerrainLagSpeed, TerrainProximity);
    const float TargetFieldOfView = FMath::Clamp(
        CameraBaseFieldOfView + (SpeedAlpha * CameraSpeedFovBoost) + (TerrainProximity * CameraTerrainFovBoost)
            - (StallSeverity * CameraStallFovPenalty),
        80.0f,
        108.0f
    );
    const FRotator TargetRotation(TargetPitch, TargetYaw, TargetRoll);

    if (DeltaTime <= KINDA_SMALL_NUMBER)
    {
        SpringArm->TargetArmLength = TargetArmLength;
        SpringArm->SocketOffset = TargetSocketOffset;
        SpringArm->CameraLagSpeed = TargetLagSpeed;
        SpringArm->SetRelativeRotation(TargetRotation);
        Camera->FieldOfView = TargetFieldOfView;
        return;
    }

    SpringArm->TargetArmLength = FMath::FInterpTo(
        SpringArm->TargetArmLength,
        TargetArmLength,
        DeltaTime,
        CameraRigBlendSpeed
    );
    SpringArm->SocketOffset = FMath::VInterpTo(
        SpringArm->SocketOffset,
        TargetSocketOffset,
        DeltaTime,
        CameraRigBlendSpeed
    );
    SpringArm->CameraLagSpeed = FMath::FInterpTo(
        SpringArm->CameraLagSpeed,
        TargetLagSpeed,
        DeltaTime,
        CameraRigBlendSpeed
    );
    SpringArm->SetRelativeRotation(FMath::RInterpTo(
        SpringArm->GetRelativeRotation(),
        TargetRotation,
        DeltaTime,
        CameraRigBlendSpeed
    ));
    Camera->FieldOfView = FMath::FInterpTo(
        Camera->FieldOfView,
        TargetFieldOfView,
        DeltaTime,
        CameraRigBlendSpeed
    );
}

void AFlightSimPawn::HandleThrottle(float Value)
{
    if (FlightMovement)
    {
        FlightMovement->SetThrottleInput(Value);
    }
}

void AFlightSimPawn::HandlePitch(float Value)
{
    if (FlightMovement)
    {
        FlightMovement->SetPitchInput(Value);
    }
}

void AFlightSimPawn::HandleRoll(float Value)
{
    if (FlightMovement)
    {
        FlightMovement->SetRollInput(Value);
    }
}

void AFlightSimPawn::HandleYaw(float Value)
{
    if (FlightMovement)
    {
        FlightMovement->SetYawInput(Value);
    }
}

void AFlightSimPawn::HandleCameraYaw(float Value)
{
    CameraYawInputValue = Value;
    if (FMath::IsNearlyZero(Value))
    {
        return;
    }

    CameraYawDegrees = FMath::Clamp(CameraYawDegrees + (Value * 1.35f), -CameraYawLimit, CameraYawLimit);
    ApplyCameraRig(0.0f);
}

void AFlightSimPawn::HandleCameraPitch(float Value)
{
    CameraPitchInputValue = Value;
    if (FMath::IsNearlyZero(Value))
    {
        return;
    }

    CameraPitchDegrees = FMath::Clamp(CameraPitchDegrees + (Value * 1.2f), CameraPitchMin, CameraPitchMax);
    ApplyCameraRig(0.0f);
}

void AFlightSimPawn::ToggleDebugHud()
{
    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (AFlightSimHUD* FlightHud = Cast<AFlightSimHUD>(PlayerController->GetHUD()))
        {
            FlightHud->ToggleOverlay();
        }
    }
}

void AFlightSimPawn::ResetFlight()
{
    SetActorLocationAndRotation(ResetLocation, ResetRotation, false, nullptr, ETeleportType::ResetPhysics);
    CameraYawDegrees = 0.0f;
    CameraPitchDegrees = CameraDefaultPitchDegrees;
    CameraYawInputValue = 0.0f;
    CameraPitchInputValue = 0.0f;
    if (FlightMovement)
    {
        FlightMovement->ResetFlightState();
    }
    if (AUnrealFlightSimGameMode* FlightGameMode = GetWorld() ? Cast<AUnrealFlightSimGameMode>(GetWorld()->GetAuthGameMode()) : nullptr)
    {
        FlightGameMode->ResetCargoMission();
    }
    ApplyCameraRig(0.0f);
}

UFlightSimMovementComponent* AFlightSimPawn::GetFlightMovement() const
{
    return FlightMovement;
}
