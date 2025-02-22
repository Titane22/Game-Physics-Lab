// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Projectile_Leviathan.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Weapons/ProjectileMaster.h"

AProjectile_Leviathan::AProjectile_Leviathan() : Super()
{
    // 부모 클래스(ProjectileMaster)의 생성자 호출
    
    // 프로젝타일 무브먼트 컴포넌트 설정
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
    if (ProjectileMovement)
    {
        ProjectileMovement->bSimulationEnabled = false;  // 시뮬레이션 비활성화
        ProjectileMovement->ProjectileGravityScale = 0.0f;  // 중력 스케일 0
        ProjectileMovement->bShouldBounce = false;  // 바운스 비활성화
        ProjectileMovement->Velocity = FVector::ZeroVector;  // 초기 속도 0
    }
}

void AProjectile_Leviathan::BeginPlay()
{
    Super::BeginPlay();  // 부모 클래스의 BeginPlay 호출 (중요!)
    
    // 추가적인 중력/물리 비활성화 설정
    if (ProjectileMovement)
    {
        ProjectileMovement->StopMovementImmediately();
        ProjectileMovement->SetComponentTickEnabled(false);
    }
}

//void AProjectile_Leviathan::Throw(FRotator CameraRotation, FVector ThrowDirectionVector, FVector CameraLocation)
//{
//    CameraStartRotation = CameraRotation;
//    ThrowDirection = ThrowDirectionVector;
//    CameraLocationAtThrow = CameraLocation;
//
//    // Snap Axe to Start Position
//    FRotator startRotation = FRotator(CameraStartRotation.Pitch, CameraStartRotation.Yaw, CameraStartRotation.Roll + AxeSpinAxisOffset);
//    SnapAxeToStartPosition(startRotation, ThrowDirection, CameraLocationAtThrow);
//
//    // Launch Axe
//    ProjectileMovement->Velocity = ThrowDirection * AxeThrowSpeed;
//    ProjectileMovement->Activate();
//
//    // TODO: Start Axe Rot Forward
//
//    AxeState = EAxeState::Launched;
//    // TODO: BeginTrails
//
//    ProjectileMovement->ProjectileGravityScale = 0.0f;
//}
//
//void AProjectile_Leviathan::SnapAxeToStartPosition(FRotator StartRotation, FVector ThrowDirectionVector, FVector CameraLocation)
//{
//    FVector newLocation = (ThrowDirectionVector * 250.0f + CameraLocation) - PivotPoint->GetRelativeLocation();
//
//    SetActorLocationAndRotation(newLocation, StartRotation);
//}
