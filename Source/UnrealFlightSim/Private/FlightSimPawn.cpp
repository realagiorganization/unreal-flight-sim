#include "FlightSimPawn.h"

#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "FlightSimMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "UObject/ConstructorHelpers.h"

AFlightSimPawn::AFlightSimPawn()
{
    PrimaryActorTick.bCanEverTick = false;
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
    SpringArm->TargetArmLength = 1050.0f;
    SpringArm->bEnableCameraLag = true;
    SpringArm->CameraLagSpeed = 3.5f;
    SpringArm->bDoCollisionTest = false;

    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

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

void AFlightSimPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    check(PlayerInputComponent);

    PlayerInputComponent->BindAxis(TEXT("Throttle"), this, &AFlightSimPawn::HandleThrottle);
    PlayerInputComponent->BindAxis(TEXT("Pitch"), this, &AFlightSimPawn::HandlePitch);
    PlayerInputComponent->BindAxis(TEXT("Roll"), this, &AFlightSimPawn::HandleRoll);
    PlayerInputComponent->BindAxis(TEXT("Yaw"), this, &AFlightSimPawn::HandleYaw);
    PlayerInputComponent->BindAxis(TEXT("CameraYaw"), this, &AFlightSimPawn::HandleCameraYaw);
    PlayerInputComponent->BindAxis(TEXT("CameraPitch"), this, &AFlightSimPawn::HandleCameraPitch);
    PlayerInputComponent->BindAction(TEXT("ResetFlight"), IE_Pressed, this, &AFlightSimPawn::ResetFlight);
}

void AFlightSimPawn::ApplyCameraRig()
{
    SpringArm->SetRelativeRotation(FRotator(CameraPitchDegrees, CameraYawDegrees, 0.0f));
}

void AFlightSimPawn::HandleThrottle(float Value)
{
    FlightMovement->SetThrottleInput(Value);
}

void AFlightSimPawn::HandlePitch(float Value)
{
    FlightMovement->SetPitchInput(Value);
}

void AFlightSimPawn::HandleRoll(float Value)
{
    FlightMovement->SetRollInput(Value);
}

void AFlightSimPawn::HandleYaw(float Value)
{
    FlightMovement->SetYawInput(Value);
}

void AFlightSimPawn::HandleCameraYaw(float Value)
{
    if (FMath::IsNearlyZero(Value))
    {
        return;
    }

    CameraYawDegrees = FMath::Clamp(CameraYawDegrees + (Value * 1.35f), -CameraYawLimit, CameraYawLimit);
    ApplyCameraRig();
}

void AFlightSimPawn::HandleCameraPitch(float Value)
{
    if (FMath::IsNearlyZero(Value))
    {
        return;
    }

    CameraPitchDegrees = FMath::Clamp(CameraPitchDegrees + (Value * 1.2f), CameraPitchMin, CameraPitchMax);
    ApplyCameraRig();
}

void AFlightSimPawn::ResetFlight()
{
    SetActorLocationAndRotation(ResetLocation, ResetRotation, false, nullptr, ETeleportType::ResetPhysics);
    CameraYawDegrees = 0.0f;
    CameraPitchDegrees = -18.0f;
    ApplyCameraRig();
    FlightMovement->ResetFlightState();
}
