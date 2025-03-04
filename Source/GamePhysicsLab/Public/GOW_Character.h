// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h" 
#include "../GamePhysicsLabCharacter.h"
#include "GOW_Character.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class AProjectile_Leviathan;
struct FInputActionValue;
/**
 * 
 */

UCLASS(config = Game)
class GAMEPHYSICSLAB_API AGOW_Character : public AGamePhysicsLabCharacter
{
	GENERATED_BODY()
	
public:
	AGOW_Character();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float ShoulderYOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float ShoulderZOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float TargetSpringAimZ;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	FVector TargetArmLengths;

	UPROPERTY()
	UTimelineComponent* AimTimeline;

	UPROPERTY()
	UTimelineComponent* DesiredSocketTimeline;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Timeline")
	class UCurveFloat* AimCurve;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Timeline")
	class UCurveFloat* DesiredSocketCurve;

	UFUNCTION()
	void UpdateAim(float Value);

	UFUNCTION()
	void UpdateDesiredSocket(float Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapons", meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* LeviathanAxe;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapons", meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* AxeCapsule;

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void Aim(const FInputActionValue& Value);

	virtual void ThrowAxe();

	virtual void ReturnAxe();

	UFUNCTION()
	void OnNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void OnNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void OnCatchNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	UFUNCTION()
	void OnCatchNotifyEnd(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPointPayload);

	// To add mapping context
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CameraTurnRate = 50.0f;

	AProjectile_Leviathan* LeviathanAxeRef;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Axe Throw", meta = (AllowPrivateAccess = "true"))
	bool IsAim;

	bool AxeThrown;
	
	bool AxeRecalling;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sound")
	USoundBase* ThrowEffortSound;

	// 타임라인 업데이트 함수
	UFUNCTION(BlueprintCallable)
	void LerpCameraPosition(float Value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Axe Throw", meta = (AllowPrivateAccess = "true"))
	float SpringArmLengthIdle = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Axe Throw", meta = (AllowPrivateAccess = "true"))
	float SpringArmLengthAim = 110.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Axe Throw", meta = (AllowPrivateAccess = "true"))
	FVector CameraVector = FVector(10.0f, 95.0f, 20.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Axe Throw", meta = (AllowPrivateAccess = "true"))
	FVector RangedCameraVector = FVector(10.0f, 80.0f, 18.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	FVector DesiredSocketOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCameraShakeBase> ShakeClass;
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	bool bUserControllerRotation;

	void Catch();
};
