// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Projectile_Leviathan.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Weapons/ProjectileMaster.h"
#include "Components/TimelineComponent.h"

AProjectile_Leviathan::AProjectile_Leviathan() 
    : Super()
{
    // 프로젝타일 무브먼트 컴포넌트 설정
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
    if (ProjectileMovement)
    {
        ProjectileMovement->bSimulationEnabled = false;  // 시뮬레이션 비활성화
        ProjectileMovement->ProjectileGravityScale = 0.0f;  // 중력 스케일 0
        ProjectileMovement->bShouldBounce = false;  // 바운스 비활성화
        ProjectileMovement->Velocity = FVector::ZeroVector;  // 초기 속도 0
    }

    AxeRotTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("AxeRotTimeline"));
    AxeThrowTraceTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("AxeThrowTraceTimeline"));
}

void AProjectile_Leviathan::BeginPlay()
{
    Super::BeginPlay();  // 부모 클래스의 BeginPlay 호출 (중요!)
    
    // 추가적인 중력/물리 비활성화 설정
    if (ProjectileMovement)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("ProjectileMovement"));
        
    }

    // 타임라인 설정
    if (AxeRotTimeline && AxeRotCurve && AxeSoundCurve)
    {
        AxeRotTimeline->SetTimelineLength(1.0f);
        AxeRotTimeline->SetPlayRate(AxeSpinRate); 
        
        FOnTimelineFloat RotCallback;
        RotCallback.BindUFunction(this, FName("UpdateAxeRotation"));
        AxeRotTimeline->AddInterpFloat(AxeRotCurve, RotCallback);

        FOnTimelineFloat AudioCallback;
        AudioCallback.BindUFunction(this, FName("UpdateAxeAudioFlipFlop"));
        AxeRotTimeline->AddInterpFloat(AxeSoundCurve, AudioCallback);
    }

    if (AxeThrowTraceTimeline && AxeTraceCurve)
    {
        AxeThrowTraceTimeline->SetTimelineLength(5.0f);

        FOnTimelineFloat TraceCallback;
        TraceCallback.BindUFunction(this, FName("UpdateAxeThrowTrace"));
        AxeThrowTraceTimeline->AddInterpFloat(AxeTraceCurve, TraceCallback);

        FOnTimelineEvent FinishCallBack;
        FinishCallBack.BindUFunction(this, FName("OnAxeThrowFinished"));
        AxeThrowTraceTimeline->SetTimelineFinishedFunc(FinishCallBack);
    }
}



void AProjectile_Leviathan::UpdateAxeRotation(float Value)
{
    if (SkeletalMesh)
    {
        FRotator newRotation = FRotator(Value * -360.0f, 0.0f, 0.0f);
        SkeletalMesh->SetRelativeRotation(newRotation);
    }
}

void AProjectile_Leviathan::UpdateAxeAudioFlipFlop(float Value)
{
    if (SkeletalMesh)
    {
        // TODO: Axe Sound 
    }
}

void AProjectile_Leviathan::UpdateAxeThrowTrace(float Value)
{
    ProjectileMovement->ProjectileGravityScale = Value;

    // 도끼의 현재 속도 벡터를 정규화하고 투척 거리만큼 확장
    FVector throwDirection = GetVelocity().GetSafeNormal(0.0001f); 
    FVector traceOffset = throwDirection * AxeThrowTraceDistance;
    
    FVector startLocation = GetActorLocation();
    FVector endLocation = startLocation + traceOffset;  // 종점 위치 계산
    //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("traceOffset: %s"), *throwDirection.ToString()));

    FCollisionQueryParams QueryParams;
    QueryParams.bTraceComplex = false;
    QueryParams.AddIgnoredActor(this);

    FHitResult HitResult;
    bool bHit = GetWorld()->LineTraceSingleByChannel(
        HitResult,
        startLocation,
        endLocation,
        ECollisionChannel::ECC_Visibility,
        QueryParams
    );
    //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("startLocation: %s, endLocation: %s"), *startLocation.ToString(), *endLocation.ToString()));
    /*if (bHit)
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Hit!!!!!!!!!!!!"));
    else
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Not Hit"));*/

    const FColor TraceColor = FColor::Red;
    const FColor HitColor = FColor::Green;
    const float DrawTime = 12.0f;           // 디버그 라인 표시 시간

    DrawDebugLine(
        GetWorld(),
        startLocation,
        endLocation,
        bHit ? HitColor : TraceColor,
        false,
        DrawTime
    );
}

void AProjectile_Leviathan::StopAxeThrowTraceTimeline()
{
    AxeThrowTraceTimeline->Stop();
}

void AProjectile_Leviathan::OnAxeThrowFinished()
{
    StopAxeMoving();

    //SkeletalMesh->SetVisibility(false);
}

void AProjectile_Leviathan::StartAxeRotForward()
{
    if (AxeRotTimeline)
    {
        ProjectileMovement->SetComponentTickEnabled(true);
        AxeRotTimeline->PlayFromStart();
    }
}

void AProjectile_Leviathan::StopAxeRotation()
{
    if (AxeRotTimeline)
    {
        ProjectileMovement->SetComponentTickEnabled(false);
        AxeRotTimeline->Stop();
    }
}

void AProjectile_Leviathan::StopAxeMoving()
{
    ProjectileMovement->Deactivate();

    StopAxeRotation();
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
    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("ProjectileMovement->Velocity: %s"), *ProjectileMovement->Velocity.ToString()));

    StartAxeRotForward();

    AxeState = EAxeState::Launched;
    // TODO: BeginTrails

    ProjectileMovement->ProjectileGravityScale = 0.0f;

    if (!AxeThrowTraceTimeline)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("AxeThrowTraceTimeline Is NULL"));
        return;
    }
    AxeThrowTraceTimeline->PlayFromStart();
    GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, TEXT("AxeThrowTraceTimeline Is Valid"));
}

void AProjectile_Leviathan::SnapAxeToStartPosition(FRotator StartRotation, FVector ThrowDirectionVector, FVector CameraLocation)
{
    FVector newLocation = (ThrowDirectionVector * 250.0f + CameraLocation) - PivotPoint->GetRelativeLocation();

    SetActorLocationAndRotation(newLocation, StartRotation);
}
