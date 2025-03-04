// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/ProjectileMaster.h"
#include "Projectile_Leviathan.generated.h"

UENUM(BlueprintType)
enum class EAxeState : uint8
{
	Idle					UMETA(DisplayName = "Idle"),
	Launched				UMETA(DisplayName = "Launched"),
	LodgedInSomething		UMETA(DisplayName = "LodgedInSomething"),
	Returning				UMETA(DisplayName = "Returning")
};

class AGOW_Character;
class UProjectileMovementComponent;
class AEnemyBase;
/**
 * 
 */
UCLASS(BlueprintType)
class GAMEPHYSICSLAB_API AProjectile_Leviathan : public AProjectileMaster
{
	GENERATED_BODY()
private:
	UProjectileMovementComponent* ProjectileMovement;

protected:
	AProjectile_Leviathan();

	virtual void BeginPlay();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle")
	UParticleSystemComponent* AxeCatchParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle")
	UParticleSystemComponent* SwingParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particle")
	UParticleSystemComponent* ThrowParticle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Axe")
	FRotator CameraStartRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Axe")
	FVector ThrowDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Axe")
	FVector CameraLocationAtThrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Axe")
	FVector ImpactLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Axe")
	FVector ImpactNormal;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Axe|Recall")
	FVector InitialLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Axe|Recall")
	FRotator InitialRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Axe|Recall")
	float OptimalDistance = 1400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Axe|Recall")
	float AxeReturnSpeed = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Axe|Recall")
	float ReturnSpinRate = 0.35f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Axe|Recall")
	int32 NumOfSpins;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Axe|Recall")
	FVector ReturnTargetLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Axe|Recall")
	FVector AxeLocationLastTick;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Axe|Recall")
	float AxeReturnRightScale = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Axe|Recall")
	float AxeReturnTilt = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs")
	FVector DefaultPivotPoint = FVector(4.332825f, 0.0f, 19.522278f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Configs")
	FVector DefaultLodgePoint = FVector(12.813232f, 0.0f, 36.75943f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* ImpactSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* Whoosh1Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* Whoosh2Sound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* DullThudSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* BrownNoiseSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* ReturnNoBrownNoiseSound;

	FName HitBoneName;

	float ZAdjustment;

	EPhysicalSurface SurfaceType;

	float AxeThrowSpeed = 2500.0f;

	float AxeSpinAxisOffset = 0.0f;

	float AxeSpinRate = 2.5f;

	float AxeThrowTraceDistance = 60.0f;

	float ImpulseStrength = 20000.0f;

	float MaxCalculationDistance = 3000.0f;

	float DistanceFromChar;

	FRotator LodgePointBaseRotation;

	EAxeState AxeState = EAxeState::Idle;

	AEnemyBase* HitAIRef;

	AGOW_Character* CharacterRef;

	void StartAxeRotForward();

	void StopAxeRotation();

	void StopAxeMoving();

	void LodgeAxe();

	void AdjustAxeReturnLocation();

	void ReturnAxe();

	UPROPERTY()
	UAudioComponent* ReturnWhoosh;

	// 타임라인 컴포넌트
	UPROPERTY()
	class UTimelineComponent* AxeRotTimeline;

	UPROPERTY()
	class UTimelineComponent* AxeThrowTraceTimeline;

	UPROPERTY()
	class UTimelineComponent* WiggleTimeline;

	UPROPERTY()
	UTimelineComponent* AxeTraceReturnTimeline;

	UPROPERTY()
	UTimelineComponent* AxeReturnTimeline;

	UPROPERTY()
	UTimelineComponent* SpinTimeline;

	UPROPERTY(EditAnywhere, Category = "Axe")
	class UCurveFloat* AxeRotCurve;

	UPROPERTY(EditAnywhere, Category = "Axe")
	class UCurveFloat* AxeSoundCurve;

	UPROPERTY(EditAnywhere, Category = "Axe")
	class UCurveFloat* AxeTraceCurve;

	UPROPERTY(EditAnywhere, Category = "Axe")
	class UCurveFloat* WiggleCurve;
	
	UPROPERTY(EditAnywhere, Category = "Axe|Recall")
	class UCurveFloat* SpinRotaionCurve;

	UPROPERTY(EditAnywhere, Category = "Axe|Recall")
	class UCurveFloat* SpinSoundCurve;

	UPROPERTY(EditAnywhere, Category = "Axe|Recall")
	class UCurveFloat* AxeRotationCurve;

	UPROPERTY(EditAnywhere, Category = "Axe|Recall")
	class UCurveFloat* AxeRightVectorCurve;

	UPROPERTY(EditAnywhere, Category = "Axe|Recall")
	class UCurveFloat* AxeRotation2Curve;

	UPROPERTY(EditAnywhere, Category = "Axe|Recall")
	class UCurveFloat* AxeReturnSpeedCurve;

	UPROPERTY(EditAnywhere, Category = "Axe|Recall")
	class UCurveFloat* AxeReturnSoundCurve;

	UPROPERTY(EditAnywhere, Category = "Axe|Recall")
	class UCurveFloat* AxeReturnTraceCurve;

	UFUNCTION()
	void UpdateAxeRotation(float Value);

	UFUNCTION()
	void UpdateAxeThrowTrace(float Value);

	UFUNCTION()
	void StopAxeThrowTraceTimeline();

	UFUNCTION()
	void OnAxeThrowFinished();

	UFUNCTION()
	void UpdateAxeWiggle(float Value);

	UFUNCTION()
	void OnAxeWiggleFinished();

	UFUNCTION()
	void UpdateAxeReturn(float Value);

	UFUNCTION()
	void OnReturnFinished();

	UFUNCTION()
	void UpdateAxeReturnSpin(float Value);

	UFUNCTION()
	void OnSpinFinished();

	UFUNCTION()
	void UpdateAxeTraceReturn(float Value);

	UFUNCTION()
	void OnWhoosh1TimelineEvent();

	UFUNCTION()
	void OnWhoosh2TimelineEvent();

public:
	void Throw(FRotator CameraRotation, FVector ThrowDirectionVector, FVector CameraLocation);

	void Recall();

	void SnapAxeToStartPosition(FRotator StartRotation, FVector ThrowDirectionVector, FVector CameraLocation);

	void SetAxeState(int32 index);
	
};
