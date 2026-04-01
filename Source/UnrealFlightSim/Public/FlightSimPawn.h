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
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

private:
    void ApplyCameraRig();
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

    float CameraYawDegrees = 0.0f;
    float CameraPitchDegrees = -18.0f;
};
