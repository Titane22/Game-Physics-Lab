// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/Projectile_Leviathan.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Weapons/ProjectileMaster.h"
#include "Components/AudioComponent.h"
#include "Components/TimelineComponent.h"
#include "Actors/DestructiblesMaster.h"
#include "GOW_Character.h"
#include "Enemy/EnemyBase.h"
#include "Kismet/KismetMathLibrary.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AProjectile_Leviathan::AProjectile_Leviathan() 
    : Super()
{
    AxeCatchParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Axe Catch Particle"));
    SwingParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Swing Particle"));
    ThrowParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Throw Particle"));
    if (PivotPoint)
    {
        PivotPoint->SetRelativeLocation(DefaultPivotPoint);
        PivotPoint->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
        PivotPoint->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
    }

    if (LodgePoint)
    {
        LodgePoint->SetRelativeLocation(DefaultLodgePoint);
        LodgePoint->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
        LodgePoint->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
    }

    if (SkeletalMesh)
    {
        SkeletalMesh->SetRelativeLocation(FVector(-17.146057f, 0.0f, -56.281708f));
        SkeletalMesh->SetRelativeRotation(FRotator(1080.0f, 0.0f, 0.000044f));
        SkeletalMesh->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
    }

    if (AxeCatchParticle)
    {
        AxeCatchParticle->SetupAttachment(SkeletalMesh);
    }

    if (SwingParticle)
    {
        SwingParticle->SetupAttachment(SkeletalMesh);
    }

    if (ThrowParticle)
    {
        ThrowParticle->SetupAttachment(SkeletalMesh);
    }

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
    AxeReturnTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("AxeReturnTimeline"));
    SpinTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("SpinTimeline"));
    AxeTraceReturnTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("AxeTraceReturnTimeline"));
    WiggleTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("WiggleTimeline"));

    static ConstructorHelpers::FObjectFinder<USoundBase> ImpactSoundAsset(TEXT("/Game/Audio/Leviathon/Impact/Impact_Cue"));
    if (ImpactSoundAsset.Succeeded())
    {
        ImpactSound = ImpactSoundAsset.Object;
    }
    
    static ConstructorHelpers::FObjectFinder<USoundBase> Whoosh1SoundAsset(TEXT("/Game/Audio/Leviathon/Spin/Whoosh1_Cue"));
    if (Whoosh1SoundAsset.Succeeded())
    {
        Whoosh1Sound = Whoosh1SoundAsset.Object;
    }

    static ConstructorHelpers::FObjectFinder<USoundBase> Whoosh2SoundAsset(TEXT("/Game/Audio/Leviathon/Spin/Whoosh2_Cue"));
    if (Whoosh2SoundAsset.Succeeded())
    {
        Whoosh2Sound = Whoosh2SoundAsset.Object;
    }

    static ConstructorHelpers::FObjectFinder<USoundBase> DullThudSoundAsset(TEXT("/Game/Audio/Leviathon/Impact/DullThud_Cue"));
    if (DullThudSoundAsset.Succeeded())
    {
        DullThudSound = DullThudSoundAsset.Object;
    }

    static ConstructorHelpers::FObjectFinder<USoundBase> BrownNoiseSoundAsset(TEXT("/Game/Audio/Leviathon/Return/BrownNoise_Cue"));
    if (BrownNoiseSoundAsset.Succeeded())
    {
        BrownNoiseSound = BrownNoiseSoundAsset.Object;
    }

    static ConstructorHelpers::FObjectFinder<USoundBase> ReturnNoBrownNoiseSoundAsset(TEXT("/Game/Audio/Leviathon/Return/Return_NoBrown_Cue"));
    if (ReturnNoBrownNoiseSoundAsset.Succeeded())
    {
        ReturnNoBrownNoiseSound = ReturnNoBrownNoiseSoundAsset.Object;
    }
}

void AProjectile_Leviathan::BeginPlay()
{
    Super::BeginPlay();  // 부모 클래스의 BeginPlay 호출 (중요!)
    
    if (AGOW_Character* PC = Cast<AGOW_Character>(GetWorld()->GetFirstPlayerController()->GetPawn()))
    {
        CharacterRef = PC;
    }

    if (AxeCatchParticle)
    {
        AxeCatchParticle->SetActorParameter(FName("VertSurfaceActor"), this);
    }

    // 타임라인 설정
    if (AxeRotTimeline && AxeRotCurve && AxeSoundCurve)
    {
        AxeRotTimeline->SetTimelineLength(1.0f);
        AxeRotTimeline->SetPlayRate(AxeSpinRate); 
        AxeRotTimeline->SetLooping(true);

        FOnTimelineFloat RotCallback;
        RotCallback.BindUFunction(this, FName("UpdateAxeRotation"));
        AxeRotTimeline->AddInterpFloat(AxeRotCurve, RotCallback, FName("Rotation"));
        AxeRotTimeline->AddInterpFloat(AxeSoundCurve, RotCallback, FName("Sound"));

        FOnTimelineEvent Sound1Callback;
        Sound1Callback.BindUFunction(this, FName("OnWhoosh1TimelineEvent"));
        AxeRotTimeline->AddEvent(0.0f, Sound1Callback);
        AxeRotTimeline->AddEvent(0.66f, Sound1Callback);

        FOnTimelineEvent Sound2Callback;
        Sound1Callback.BindUFunction(this, FName("OnWhoosh2TimelineEvent"));
        AxeRotTimeline->AddEvent(0.33f, Sound2Callback);
    }

    if (AxeThrowTraceTimeline && AxeTraceCurve)
    {
        AxeThrowTraceTimeline->SetTimelineLength(5.0f);
        AxeThrowTraceTimeline->SetLooping(true);

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
        WiggleTimeline->SetPlayRate(3.5f);

        FOnTimelineFloat WiggleCallback;
        WiggleCallback.BindUFunction(this, FName("UpdateAxeWiggle"));
        WiggleTimeline->AddInterpFloat(WiggleCurve, WiggleCallback);

        FOnTimelineEvent FinishCallback;
        FinishCallback.BindUFunction(this, FName("OnAxeWiggleFinished"));
        WiggleTimeline->SetTimelineFinishedFunc(FinishCallback);
    }

    if (AxeReturnTimeline && AxeRotationCurve && AxeRightVectorCurve && AxeReturnSpeedCurve && AxeRotation2Curve/* && AxeReturnSoundCurve*/)
    {
        AxeReturnTimeline->SetTimelineLength(1.0f);

        FOnTimelineFloat UpdateCallback;
        UpdateCallback.BindUFunction(this, FName("UpdateAxeReturn"));
        AxeReturnTimeline->AddInterpFloat(AxeRotationCurve, UpdateCallback, FName("Rotation1"));
        AxeReturnTimeline->AddInterpFloat(AxeRightVectorCurve, UpdateCallback, FName("Right Vector"));
        AxeReturnTimeline->AddInterpFloat(AxeReturnSpeedCurve, UpdateCallback, FName("Return Speed"));
        AxeReturnTimeline->AddInterpFloat(AxeRotation2Curve, UpdateCallback, FName("Rotation2"));
        AxeReturnTimeline->AddInterpFloat(AxeReturnSoundCurve, UpdateCallback, FName("Sound"));

        FOnTimelineEvent FinishedCallback;
        FinishedCallback.BindUFunction(this, FName("OnReturnFinished"));
        AxeReturnTimeline->SetTimelineFinishedFunc(FinishedCallback);
    }

    if (SpinTimeline && SpinRotaionCurve)
    {
        SpinTimeline->SetTimelineLength(1.0f);

        FOnTimelineFloat SpinCallback;
        SpinCallback.BindUFunction(this, FName("UpdateAxeReturnSpin"));
        SpinTimeline->AddInterpFloat(SpinRotaionCurve, SpinCallback);

        FOnTimelineEvent SpinFinishedCallback;
        SpinFinishedCallback.BindUFunction(this, FName("OnSpinFinished"));
        SpinTimeline->SetTimelineFinishedFunc(SpinFinishedCallback);

        FOnTimelineEvent Sound1Callback;
        Sound1Callback.BindUFunction(this, FName("OnWhoosh1TimelineEvent"));
        SpinTimeline->AddEvent(0.33f, Sound1Callback);


        FOnTimelineEvent Sound2Callback;
        Sound1Callback.BindUFunction(this, FName("OnWhoosh2TimelineEvent"));
        SpinTimeline->AddEvent(0.0f, Sound2Callback);
        SpinTimeline->AddEvent(0.66f, Sound2Callback);
    }

    if (AxeTraceReturnTimeline && AxeReturnTraceCurve)
    {
        AxeTraceReturnTimeline->SetTimelineLength(0.01f);
        AxeTraceReturnTimeline->SetLooping(true);

        FOnTimelineFloat UpdateCallback;
        UpdateCallback.BindUFunction(this, FName("UpdateAxeTraceReturn"));
        AxeTraceReturnTimeline->AddInterpFloat(AxeReturnTraceCurve, UpdateCallback);
    }
}



void AProjectile_Leviathan::UpdateAxeRotation(float Value)
{
    float currentTime = AxeRotTimeline->GetPlaybackPosition();

    float rotationValue = AxeRotCurve ? AxeRotCurve->GetFloatValue(currentTime) : 0.0f;

    bool shouldTrigger = false;

    if (SkeletalMesh)
    {
        FRotator newRotation = FRotator(rotationValue * -360.0f, 0.0f, 0.0f);
        PivotPoint->SetRelativeRotation(newRotation);
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

    if (bHit)
    {
        ImpactLocation = HitResult.ImpactPoint;
        ImpactNormal = HitResult.ImpactNormal;
        HitBoneName = HitResult.BoneName;
        if (HitResult.PhysMaterial != nullptr)
        {
            SurfaceType = HitResult.PhysMaterial->SurfaceType;
        }

        if (ADestructiblesMaster* DestructiblesMaster = Cast<ADestructiblesMaster>(HitResult.GetActor()))// TODO: HitResult.PhysMaterial
        {
            DestructiblesMaster->BreakObject(ImpactLocation, ThrowDirection);
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Actor: %s"), *HitResult.GetActor()->GetName()));
            if (ThrowParticle)
            {
                ThrowParticle->EndTrails();
            }
            StopAxeThrowTraceTimeline();
            ProjectileMovement->Deactivate();
            if (AEnemyBase* EnemyBase = Cast<AEnemyBase>(HitResult.GetActor()))
            {
                HitAIRef = EnemyBase;
                LodgeAxe();

                // 이미지의 노드 구현: 타겟 액터 위치 가져오기 및 정규화
                FVector targetLocation = HitAIRef->GetActorLocation();
                FVector directionVector = targetLocation - CameraLocationAtThrow;
                FVector normalizedDirection = directionVector.GetSafeNormal(0.0001f);
                FVector impactVector = normalizedDirection * ImpulseStrength;

                HitAIRef->ReceiveHit(true, HitBoneName, impactVector);
                FAttachmentTransformRules attachRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, false);

                attachRules.bWeldSimulatedBodies = true;
                this->AttachToComponent(HitAIRef->GetMesh(), attachRules, HitBoneName);

                // TODO: VFX
            }
            else
            {
                UGameplayStatics::PlaySoundAtLocation(
                    this,
                    ImpactSound,  // 초기화된 사운드 사용
                    ImpactLocation
                );
                UGameplayStatics::PlaySoundAtLocation(
                    this,
                    DullThudSound,  // 초기화된 사운드 사용
                    ImpactLocation
                );

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

    SkeletalMesh->SetVisibility(false);
}

void AProjectile_Leviathan::UpdateAxeWiggle(float Value)
{
    float roll = Value * 12.0f + LodgePointBaseRotation.Roll;
    LodgePoint->SetRelativeRotation(FRotator(LodgePointBaseRotation.Pitch, LodgePointBaseRotation.Yaw, roll));
}

void AProjectile_Leviathan::OnAxeWiggleFinished()
{
    if (ThrowParticle)
    {
        ThrowParticle->BeginTrails(FName("BaseSocket"), FName("TipSocket"), ETrailWidthMode_FromCentre, 1.0f);
    }
    ReturnAxe();
}

void AProjectile_Leviathan::UpdateAxeReturn(float Value)
{
    float currentTime = AxeReturnTimeline->GetPlaybackPosition();

    float rotation1Value = AxeRotationCurve ? AxeRotationCurve->GetFloatValue(currentTime) : 0.0f;
    float rotation2Value = AxeRotation2Curve ? AxeRotation2Curve->GetFloatValue(currentTime) : 0.0f;
    float speedValue = AxeReturnSpeedCurve ? AxeReturnSpeedCurve->GetFloatValue(currentTime) : 0.0f;
    float rightVectorValue = AxeRightVectorCurve ? AxeRightVectorCurve->GetFloatValue(currentTime) : 0.0f;
    float soundValue = AxeReturnSoundCurve ? AxeReturnSoundCurve->GetFloatValue(currentTime) : 0.0f;

    float rightVectorOffset = rightVectorValue * (DistanceFromChar / AxeReturnRightScale);
    FVector cameraRightDirection = CharacterRef->GetFollowCamera()->GetRightVector() * rightVectorOffset;
    FVector targetSocketLocation = CharacterRef->GetMesh()->GetSocketLocation(FName("AxeSocket"));

    ReturnTargetLocation = FMath::Lerp(InitialLocation, targetSocketLocation + cameraRightDirection, speedValue);

    FRotator targetReturnRotation = FRotator(CameraStartRotation.Pitch, CameraStartRotation.Yaw, CameraStartRotation.Roll + AxeReturnTilt);
    FRotator lerpedRotation = UKismetMathLibrary::RLerp(InitialRotation, targetReturnRotation, rotation1Value, true);
    FRotator newRotation = UKismetMathLibrary::RLerp(lerpedRotation, CharacterRef->GetMesh()->GetSocketRotation(FName("AxeSocket")), rotation2Value, true);

    SetActorLocationAndRotation(ReturnTargetLocation, newRotation);
    
    if (ReturnWhoosh)
    {
        ReturnWhoosh->SetVolumeMultiplier(soundValue);
    }
}

void AProjectile_Leviathan::OnReturnFinished()
{
    AxeTraceReturnTimeline->Stop();

    if (ReturnWhoosh)
        ReturnWhoosh->FadeOut(0.4f, 0.0f);
    else
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("ReturnWhoosh is NULL"));

    StopAxeMoving();

    CharacterRef->Catch();
    
    if (AxeCatchParticle)
    {
        FTimerHandle catchTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            catchTimerHandle,
            [this]() {
                AxeCatchParticle->Activate();
            },
            0.05f,
            false
        );
    }

    if (ThrowParticle)
    {
        FTimerHandle throwTimerHandle;
        GetWorld()->GetTimerManager().SetTimer(
            throwTimerHandle,
            [this]() {
                ThrowParticle->EndTrails();
            },
            0.07f,
            false
        );
    }
}

void AProjectile_Leviathan::UpdateAxeReturnSpin(float Value)
{
    float currentTime = SpinTimeline->GetPlaybackPosition();
    bool shouldTrigger = false;

    PivotPoint->SetRelativeRotation(FRotator(Value * 360.0f, 0.0f, 0.0f));

}

void AProjectile_Leviathan::OnSpinFinished()
{
    NumOfSpins = NumOfSpins - 1;

    if (NumOfSpins > 0)
    {
        SpinTimeline->PlayFromStart();
    }
}

void AProjectile_Leviathan::UpdateAxeTraceReturn(float Value)
{
    FHitResult HitResult;
    bool bHit = UKismetSystemLibrary::SphereTraceSingle(
        this,
        ReturnTargetLocation,
        AxeLocationLastTick,
        25.0f,
        UEngineTypes::ConvertToTraceType(ECollisionChannel::ECC_Visibility),
        false,
        TArray<AActor*>(),
        EDrawDebugTrace::ForDuration,
        HitResult,
        true
    );

    AxeLocationLastTick = ReturnTargetLocation;

    if (bHit)
    {
        ImpactLocation = HitResult.ImpactPoint;
        if (ADestructiblesMaster* DestructibleActor = Cast<ADestructiblesMaster>(HitResult.GetActor()))
        {
            GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Green, TEXT("TODO: Break Object"));
        }
        else
        {
            if (AEnemyBase* Enemy = Cast<AEnemyBase>(HitResult.GetActor()))
            {
                GEngine->AddOnScreenDebugMessage(-1, 2.5f, FColor::Green, TEXT("TODO: Receive Hit"));
            }
        }
    }
}

void AProjectile_Leviathan::OnWhoosh1TimelineEvent()
{
    UGameplayStatics::SpawnSoundAttached(
        Whoosh1Sound,
        SkeletalMesh
    );
}

void AProjectile_Leviathan::OnWhoosh2TimelineEvent()
{
    UGameplayStatics::SpawnSoundAttached(
        Whoosh2Sound,
        SkeletalMesh
    );
}

void AProjectile_Leviathan::StartAxeRotForward()
{
    if (AxeRotTimeline)
    {
        //ProjectileMovement->SetComponentTickEnabled(true);
        AxeRotTimeline->SetPlayRate(AxeSpinRate);
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
        FVector(1.0f, 1.0f, 1.0f),
        FVector(1.0f, 1.0f, 1.0f)
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

    float roll = FMath::FRandRange(-3.0f, -8.0f);
    LodgePoint->SetRelativeRotation(FRotator(pitch, 0.0f, roll));

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

    ReturnWhoosh = UGameplayStatics::SpawnSoundAttached(
        BrownNoiseSound,
        SkeletalMesh
    );

    switch (AxeState)
    {
    case EAxeState::Launched:
        ZAdjustment = 10.0f;
        break;
    case EAxeState::LodgedInSomething:
        LodgePointBaseRotation = LodgePoint->GetRelativeRotation();
        WiggleTimeline->PlayFromStart();
        break;
    }
    ReturnAxe();
}

void AProjectile_Leviathan::AdjustAxeReturnLocation()
{
    FVector actorLocation = GetActorLocation();
    float scaledOffset = 30 * (1.0f - ZAdjustment / 10.0f);
    FVector offsetZ = FVector(0.0f, 0.0f, scaledOffset + 20.0f);
    
    FVector newLocation = actorLocation + offsetZ;
    SetActorLocation(newLocation);
}

void AProjectile_Leviathan::ReturnAxe()
{
    AxeState = EAxeState::Returning;
    
    // 도끼의 현재 위치에서 캐릭터의 도끼 소켓 위치까지의 벡터를 계산
    FVector directionToCharacter = GetActorLocation() - CharacterRef->GetMesh()->GetSocketLocation(FName("AxeSocket"));
    
    DistanceFromChar = FMath::Clamp(directionToCharacter.Length(), 0.0f, MaxCalculationDistance);
    //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("DistanceFromChar: %lf"), DistanceFromChar));
    AdjustAxeReturnLocation();

    InitialLocation = GetActorLocation();
    InitialRotation = GetActorRotation();
    CameraStartRotation = CharacterRef->GetFollowCamera()->GetComponentRotation();

    LodgePoint->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
    
    // Timeline
    float newRate = FMath::Clamp(OptimalDistance * AxeReturnSpeed / DistanceFromChar, 0.4f, 7.0f);
    AxeReturnTimeline->SetPlayRate(newRate);
    /// start of Return Position and Return Audio
    AxeReturnTimeline->PlayFromStart();
    /// end of Return Position and Return Audio
    /// 
    /// start of Return Spin and return audio
    float timelineLength = 1.0f / newRate;
    
    FTimerHandle TimeHandle;
    if (timelineLength - 0.87f > 0.0f)
    {
        GetWorldTimerManager().SetTimer(
            TimeHandle,
            [this, timelineLength]() {
                GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("timelineLength - 0.87f > 0.0f"));
                UGameplayStatics::SpawnSoundAttached(
                    ReturnNoBrownNoiseSound,
                    SkeletalMesh
                );
            },
            timelineLength - 0.87f,
            false
        );
    }
    else
    {
        float startTimeLength = 0.87f - timelineLength;
        
        //GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("timelineLength - 0.87f <= 0.0f"));
        GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT(" 0.87f - timelineLength: %lf"), 0.87f - timelineLength));
        if (UAudioComponent* audioComp = UGameplayStatics::SpawnSoundAttached(
            ReturnNoBrownNoiseSound,            // 사운드
            SkeletalMesh,                       // 부착할 컴포넌트
            NAME_None,                          // 소켓 이름 (기본값)
            FVector::ZeroVector,                // 위치 (기본값)
            FRotator::ZeroRotator,              // 회전 (기본값)
            EAttachLocation::KeepRelativeOffset, // 부착 규칙 (기본값)
            true,                               // 자동 파괴 (기본값)
            1.0f,                               // 볼륨 (기본값)
            startTimeLength                     // 시작 시간 (지정)
        ))
        {
            audioComp->FadeIn(0.1f, 1.0f, startTimeLength);
        }
    }
    StopAxeRotation();

    NumOfSpins = FMath::RoundToInt32(timelineLength / ReturnSpinRate);
    float spinLength = (timelineLength - 0.055f) / (float)NumOfSpins;
    
    SpinTimeline->SetPlayRate(1.0f / spinLength);
    SpinTimeline->PlayFromStart();

    AxeLocationLastTick = ReturnTargetLocation;
    AxeTraceReturnTimeline->PlayFromStart();
    /// end of Return Spin and return audio
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

    ProjectileMovement->Velocity = ThrowDirection * AxeThrowSpeed;
    ProjectileMovement->Activate();

    StartAxeRotForward();

    AxeState = EAxeState::Launched;
    // BeginTrails
    if (ThrowParticle)
    {
        ThrowParticle->BeginTrails(FName("BaseSocket"), FName("TipSocket"), ETrailWidthMode_FromCentre, 1.0f);
    }
    /// 2025/02/25 - ProjectileMovement Activating
    //ProjectileMovement->Velocity = ThrowDirectionVector * AxeThrowSpeed;
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

void AProjectile_Leviathan::SetAxeState(int32 index)
{
    switch (index)
    {
    case 0:
        AxeState = EAxeState::Idle;
        break;
    case 1:
        AxeState = EAxeState::Launched;
        break;
    case 2:
        AxeState = EAxeState::LodgedInSomething;
        break;
    case 3:
        AxeState = EAxeState::Returning;
        break;
    }
}
