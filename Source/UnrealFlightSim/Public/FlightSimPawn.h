#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "FlightSimPawn.generated.h"

class UCameraComponent;
class UFlightSimMovementComponent;
class USpringArmComponent;
class UStaticMeshComponent;
class USceneComponent;
class UInputComponent;

UCLASS()
class UNREALFLIGHTSIM_API AFlightSimPawn : public APawn
{
    GENERATED_BODY()

public:
    AFlightSimPawn();

    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
    void ApplyCameraRig(float DeltaTime);
    void HandleThrottle(float Value);
    void HandlePitch(float Value);
    void HandleRoll(float Value);
    void HandleYaw(float Value);
    void HandleCameraYaw(float Value);
    void HandleCameraPitch(float Value);
    void ResetFlight();

    UPROPERTY(VisibleAnywhere, Category="Components")
    TObjectPtr<USceneComponent> SceneRoot;

    UPROPERTY(VisibleAnywhere, Category="Components")
    TObjectPtr<UStaticMeshComponent> FuselageMesh;

    UPROPERTY(VisibleAnywhere, Category="Components")
    TObjectPtr<UStaticMeshComponent> WingMesh;

    UPROPERTY(VisibleAnywhere, Category="Components")
    TObjectPtr<UStaticMeshComponent> TailPlaneMesh;

    UPROPERTY(VisibleAnywhere, Category="Components")
    TObjectPtr<UStaticMeshComponent> VerticalFinMesh;

    UPROPERTY(VisibleAnywhere, Category="Components")
    TObjectPtr<USpringArmComponent> SpringArm;

    UPROPERTY(VisibleAnywhere, Category="Components")
    TObjectPtr<UCameraComponent> Camera;

    UPROPERTY(VisibleAnywhere, Category="Components")
    TObjectPtr<UFlightSimMovementComponent> FlightMovement;

    UPROPERTY(EditAnywhere, Category="Flight")
    FVector ResetLocation = FVector(0.0f, 0.0f, 18000.0f);

    UPROPERTY(EditAnywhere, Category="Flight")
    FRotator ResetRotation = FRotator(0.0f, 0.0f, 0.0f);

    UPROPERTY(EditAnywhere, Category="Camera")
    float CameraPitchMin = -75.0f;

    UPROPERTY(EditAnywhere, Category="Camera")
    float CameraPitchMax = -5.0f;

    UPROPERTY(EditAnywhere, Category="Camera")
    float CameraYawLimit = 95.0f;

    UPROPERTY(EditAnywhere, Category="Camera")
    float CameraDefaultPitchDegrees = -18.0f;

    UPROPERTY(EditAnywhere, Category="Camera|BushFlying")
    float CameraYawReturnSpeed = 1.9f;

    UPROPERTY(EditAnywhere, Category="Camera|BushFlying")
    float CameraPitchReturnSpeed = 1.4f;

    UPROPERTY(EditAnywhere, Category="Camera|BushFlying")
    float CameraRigBlendSpeed = 4.5f;

    UPROPERTY(EditAnywhere, Category="Camera|BushFlying")
    float CameraBaseArmLength = 1050.0f;

    UPROPERTY(EditAnywhere, Category="Camera|BushFlying")
    float CameraSpeedArmExtension = 220.0f;

    UPROPERTY(EditAnywhere, Category="Camera|BushFlying")
    float CameraTerrainArmCompression = 280.0f;

    UPROPERTY(EditAnywhere, Category="Camera|BushFlying")
    float CameraStallArmCompression = 160.0f;

    UPROPERTY(EditAnywhere, Category="Camera|BushFlying")
    float CameraTerrainPitchBiasDegrees = -9.0f;

    UPROPERTY(EditAnywhere, Category="Camera|BushFlying")
    float CameraStallPitchBiasDegrees = 5.0f;

    UPROPERTY(EditAnywhere, Category="Camera|BushFlying")
    float CameraTurnLookFactor = 0.18f;

    UPROPERTY(EditAnywhere, Category="Camera|BushFlying")
    float CameraBankFollow = 0.14f;

    UPROPERTY(EditAnywhere, Category="Camera|BushFlying")
    float CameraTerrainSocketLift = 95.0f;

    UPROPERTY(EditAnywhere, Category="Camera|BushFlying")
    float CameraStallSocketLift = 40.0f;

    UPROPERTY(EditAnywhere, Category="Camera|BushFlying")
    float CameraBaseLagSpeed = 3.5f;

    UPROPERTY(EditAnywhere, Category="Camera|BushFlying")
    float CameraTerrainLagSpeed = 5.2f;

    UPROPERTY(EditAnywhere, Category="Camera|BushFlying")
    float CameraBaseFieldOfView = 88.0f;

    UPROPERTY(EditAnywhere, Category="Camera|BushFlying")
    float CameraSpeedFovBoost = 6.0f;

    UPROPERTY(EditAnywhere, Category="Camera|BushFlying")
    float CameraTerrainFovBoost = 7.0f;

    UPROPERTY(EditAnywhere, Category="Camera|BushFlying")
    float CameraStallFovPenalty = 4.0f;

    float CameraYawDegrees = 0.0f;
    float CameraPitchDegrees = -18.0f;
    float CameraYawInputValue = 0.0f;
    float CameraPitchInputValue = 0.0f;
};
