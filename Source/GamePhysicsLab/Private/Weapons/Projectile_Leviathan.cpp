// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Projectile_Leviathan.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Weapons/ProjectileMaster.h"
#include "Components/TimelineComponent.h"
#include "Actors/DestructiblesMaster.h"
#include "GOW_Character.h"
#include "Enemy/EnemyBase.h"
#include "Kismet/KismetMathLibrary.h"

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
    WiggleTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("WiggleTimeline"));
}

void AProjectile_Leviathan::BeginPlay()
{
    Super::BeginPlay();  // 부모 클래스의 BeginPlay 호출 (중요!)
    
    if (AGOW_Character* PC = Cast<AGOW_Character>(GetWorld()->GetFirstPlayerController()->GetPawn()))
    {
        CharacterRef = PC;
    }

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

    if (WiggleTimeline && WiggleCurve)
    {
        WiggleTimeline->SetTimelineLength(0.6f);

        FOnTimelineFloat WiggleCallback;
        WiggleCallback.BindUFunction(this, FName("UpdateAxeWiggle"));
        WiggleTimeline->AddInterpFloat(WiggleCurve, WiggleCallback);

        FOnTimelineEvent FinishCallback;
        FinishCallback.BindUFunction(this, FName("OnAxeWiggleFinished"));
        WiggleTimeline->SetTimelineFinishedFunc(FinishCallback);
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

    // 현재 위치와 속도 디버그 출력
    FVector CurrentLocation = GetActorLocation();
    FVector CurrentVelocity = GetVelocity();
    
    GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Green, 
        FString::Printf(TEXT("Location: X=%.2f Y=%.2f Z=%.2f"), 
        CurrentLocation.X, CurrentLocation.Y, CurrentLocation.Z));
    
    GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow, 
        FString::Printf(TEXT("Velocity: X=%.2f Y=%.2f Z=%.2f"), 
        CurrentVelocity.X, CurrentVelocity.Y, CurrentVelocity.Z));

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
        ECollisionChannel::ECC_Pawn,
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

    if (bHit)
    {
        ImpactLocation = HitResult.ImpactPoint;
        ImpactNormal = HitResult.ImpactNormal;
        HitBoneName = HitResult.BoneName;
        if (HitResult.PhysMaterial != nullptr)
        {
            SurfaceType = HitResult.PhysMaterial->SurfaceType;
        }

        if (ADestructiblesMaster* DestructiblesMaster = Cast<ADestructiblesMaster>(HitResult.PhysMaterial))
        {
            DestructiblesMaster->BreakObject(ImpactLocation, ThrowDirection);
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Actor: %s"), *HitResult.GetActor()->GetName()));
            // TODO: EndTrails
            StopAxeThrowTraceTimeline();
            ProjectileMovement->Deactivate();
            if (AEnemyBase* EnemyBase = Cast<AEnemyBase>(HitResult.GetActor()))
            {
                HitAIRef = EnemyBase;
                LodgeAxe();

                // 이미지의 노드 구현: 타겟 액터 위치 가져오기 및 정규화
                FVector targetLocation = HitAIRef->GetActorLocation();
                FVector directionVector = targetLocation - CameraLocationAtThrow;
                FVector normalizedDirection = directionVector.GetSafeNormal(0.0001f); // TODO: Need to Modify Direction
                FVector impactVector = normalizedDirection * ImpulseStrength;

                HitAIRef->ReceiveHit(true, HitBoneName, impactVector);
                FAttachmentTransformRules attachRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false);

                attachRules.bWeldSimulatedBodies = true;
                this->AttachToComponent(HitAIRef->GetMesh(), attachRules, HitBoneName);

                // TODO: VFX
            }
            else
            {
                // TODO: Spawn Sound

                LodgeAxe();
            }
        }
    }
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

void AProjectile_Leviathan::UpdateAxeWiggle(float Value)
{
    float roll = Value * 12.0f + LodgePointBaseRotation.Roll;

    LodgePoint->SetRelativeRotation(FRotator(LodgePointBaseRotation.Pitch, LodgePointBaseRotation.Yaw, roll));
}

void AProjectile_Leviathan::OnAxeWiggleFinished()
{
    // TODO: Begin Trail
    ReturnAxe();
}

void AProjectile_Leviathan::StartAxeRotForward()
{
    if (AxeRotTimeline)
    {
        //ProjectileMovement->SetComponentTickEnabled(true);
        AxeRotTimeline->PlayFromStart();
    }
}

void AProjectile_Leviathan::StopAxeRotation()
{
    if (AxeRotTimeline)
    {
        //ProjectileMovement->SetComponentTickEnabled(false);
        AxeRotTimeline->Stop();
    }
}

void AProjectile_Leviathan::StopAxeMoving()
{
    ProjectileMovement->Deactivate();

    StopAxeRotation();
}

void AProjectile_Leviathan::LodgeAxe()
{
    StopAxeMoving();
    PivotPoint->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

    SetActorRotation(CameraStartRotation);

    float inclinedSurfaceRange = FMath::FRandRange(-30.0f, -55.0f);
    float regularSurfaceRange = FMath::FRandRange(-5.0f, -25.0f);
    FRotator rotationFromAxes = UKismetMathLibrary::MakeRotationFromAxes(
        ImpactNormal,
        FVector(0.0f, 0.0f, 0.0f),
        FVector(0.0f, 0.0f, 0.0f)
    );
    float pitch = 0.0f;
    if (rotationFromAxes.Pitch > 0.0f)
    {
        pitch = regularSurfaceRange - rotationFromAxes.Pitch;
    }
    else
    {
        pitch = inclinedSurfaceRange - rotationFromAxes.Pitch;
    }

    float roll = FMath::FRandRange(-3.0f, 3.0f);
    LodgePoint->SetRelativeRotation(FRotator(roll, pitch, 0.0f));

    pitch = 0.0f;
    if (rotationFromAxes.Pitch > 0.0f)
    {
        pitch = rotationFromAxes.Pitch;
    }

    ZAdjustment = (90.0f - pitch) / 90.0f * 10.0f;

    FVector targetLocation = ImpactLocation + FVector(0.0f, 0.0f, ZAdjustment);
    FVector locationOffset = GetActorLocation() - LodgePoint->GetComponentLocation();
    FVector newLocation = targetLocation + locationOffset;

    SetActorLocation(newLocation);
    AxeState = EAxeState::LodgedInSomething;
}

void AProjectile_Leviathan::Recall()
{
    StopAxeThrowTraceTimeline();

    SkeletalMesh->SetVisibility(true);

    // TODO: Audio

    switch (AxeState)
    {
    case EAxeState::Launched:
        ZAdjustment = 10.0f;
        ReturnAxe();
        break;
    case EAxeState::LodgedInSomething:
        // TODO: AxeLodgePull
        
        break;
    }
}

void AProjectile_Leviathan::AxeLodgeWiggle()
{
    LodgePointBaseRotation = LodgePoint->GetRelativeRotation();

    
}

void AProjectile_Leviathan::ReturnAxe()
{
    AxeState = EAxeState::Returning;
    
    // 도끼의 현재 위치에서 캐릭터의 도끼 소켓 위치까지의 벡터를 계산
    FVector DirectionToCharacter = GetActorLocation() - CharacterRef->GetMesh()->GetSocketLocation(FName("AxeSocket"));

    DistanceFromChar = FMath::Clamp(DirectionToCharacter.Length(), 0.0f, MaxCalculationDistance);
    // TODO: AdjustAxeReturnLocation();
}

void AProjectile_Leviathan::Throw(FRotator CameraRotation, FVector ThrowDirectionVector, FVector CameraLocation)
{
    if (!ProjectileMovement)
        return;

    CameraStartRotation = CameraRotation;
    ThrowDirection = ThrowDirectionVector;
    CameraLocationAtThrow = CameraLocation;

    // Snap Axe to Start Position
    FRotator startRotation = FRotator(CameraStartRotation.Pitch, CameraStartRotation.Yaw, CameraStartRotation.Roll + AxeSpinAxisOffset);
    SnapAxeToStartPosition(startRotation, ThrowDirection, CameraLocationAtThrow);

    ProjectileMovement->Activate();

    StartAxeRotForward();

    AxeState = EAxeState::Launched;
    // TODO: BeginTrails
    /// 2025/02/25 - ProjectileMovement Activating
    ProjectileMovement->Velocity = ThrowDirectionVector * AxeThrowSpeed;
    ProjectileMovement->bSimulationEnabled = true;
    ///
    ProjectileMovement->ProjectileGravityScale = 0.0f;

    if (!AxeThrowTraceTimeline)
    {
        GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("AxeThrowTraceTimeline Is NULL"));
        return;
    }
    AxeThrowTraceTimeline->PlayFromStart();
}

void AProjectile_Leviathan::SnapAxeToStartPosition(FRotator StartRotation, FVector ThrowDirectionVector, FVector CameraLocation)
{
    FVector newLocation = (ThrowDirectionVector * 250.0f + CameraLocation) - PivotPoint->GetRelativeLocation();

    SetActorLocationAndRotation(newLocation, StartRotation);
}
