// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Projectile_Leviathan.h"
#include "GameFramework/ProjectileMovementComponent.h"

AProjectile_Leviathan::AProjectile_Leviathan()
    :Super()
{
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
    ProjectileMovement->bSimulationUseScopedMovement = true;
    ProjectileMovement->bSweepCollision = true;

}

void AProjectile_Leviathan::BeginPlay()
{
    Super::BeginPlay();
}

void AProjectile_Leviathan::Throw(FRotator CameraRotation, FVector ThrowDirectionVector, FVector CameraLocation)
{
    CameraStartRotation = CameraRotation;
    ThrowDirection = ThrowDirectionVector;
    CameraLocationAtThrow = CameraLocation;

    // Snap Axe to Start Position
    FRotator startRotation = FRotator(CameraStartRotation.Pitch, CameraStartRotation.Yaw, CameraStartRotation.Roll + AxeSpinAxisOffset);
    SnapAxeToStartPosition(startRotation, ThrowDirection, CameraLocationAtThrow);

    // Launch Axe
    ProjectileMovement->Velocity = ThrowDirection * AxeThrowSpeed;
    ProjectileMovement->Activate();

    // TODO: Start Axe Rot Forward

    AxeState = EAxeState::Launched;
    // TODO: BeginTrails

    ProjectileMovement->ProjectileGravityScale = 0.0f;
}

void AProjectile_Leviathan::SnapAxeToStartPosition(FRotator StartRotation, FVector ThrowDirectionVector, FVector CameraLocation)
{
    FVector newLocation = (ThrowDirectionVector * 250.0f + CameraLocation) - PivotPoint->GetRelativeLocation();

    SetActorLocationAndRotation(newLocation, StartRotation);
}
