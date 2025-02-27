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
UCLASS()
class GAMEPHYSICSLAB_API AProjectile_Leviathan : public AProjectileMaster
{
	GENERATED_BODY()
private:
	UProjectileMovementComponent* ProjectileMovement;

protected:
	AProjectile_Leviathan();

	virtual void BeginPlay();

	FRotator CameraStartRotation;

	FVector ThrowDirection;

	FVector CameraLocationAtThrow;

	FVector ImpactLocation;

	FVector ImpactNormal;

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

	void Recall();

	void AxeLodgeWiggle();

	void ReturnAxe();
	// 타임라인 컴포넌트
	UPROPERTY()
	class UTimelineComponent* AxeRotTimeline;

	UPROPERTY()
	class UTimelineComponent* AxeThrowTraceTimeline;

	UPROPERTY()
	class UTimelineComponent* WiggleTimeline;

	UPROPERTY(EditAnywhere, Category = "Axe")
	class UCurveFloat* AxeRotCurve;

	UPROPERTY(EditAnywhere, Category = "Axe")
	class UCurveFloat* AxeSoundCurve;

	UPROPERTY(EditAnywhere, Category = "Axe")
	class UCurveFloat* AxeTraceCurve;

	UPROPERTY(EditAnywhere, Category = "Axe")
	class UCurveFloat* WiggleCurve;
	
	UFUNCTION()
	void UpdateAxeRotation(float Value);

	UFUNCTION()
	void UpdateAxeAudioFlipFlop(float Value);

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

public:
	void Throw(FRotator CameraRotation, FVector ThrowDirectionVector, FVector CameraLocation);

	void SnapAxeToStartPosition(FRotator StartRotation, FVector ThrowDirectionVector, FVector CameraLocation);
};
