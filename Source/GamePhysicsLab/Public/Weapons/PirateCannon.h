// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "InputActionValue.h"
#include "PirateCannon.generated.h"

class ACannonBall;
class APirateCharacter;
class UInputMappingContext;
class UInputAction;
class UEnhancedInputComponent;

UCLASS()
class GAMEPHYSICSLAB_API APirateCannon : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APirateCannon();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* CanonMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* RotateYawAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* RotatePitchAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input", meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;

	UFUNCTION(BlueprintCallable, Category = "Cannon")
	virtual void Interact();

	UFUNCTION(BlueprintCallable, Category = "Cannon")
	virtual void RotateYaw(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Cannon")
	virtual void RotatePitch(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Cannon")
	virtual void ActivateCannonView(APirateCharacter* Cahracter);

	UFUNCTION(BlueprintCallable, Category = "Cannon")
	virtual void DeactivateCannonView();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cannon")
	bool bIsPlayerControlling;

	UFUNCTION(BlueprintCallable, Category = "Cannon")
	virtual void Fire();

	UFUNCTION(BlueprintCallable, Category = "Cannon")
	virtual void Reload();

	void OnCannonBallHit();

protected:
	virtual void PossessedBy(AController* NewController) override;

	virtual void UnPossessed() override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* DefaultComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* CannonBaseComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* CannonBarrelComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* InteractComponet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* MuzzleLocation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CannonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Configs")
	APirateCharacter* CharacterRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cannon")
	TSubclassOf<ACannonBall> CannonBallClass;

	float RotationSpeed = 3000.0f;

	float MaxYawAngle = 30.0f;

	float MaxPitchAngle = 45.0f;

	float CurrentYawVelocity = 0.0f;

	float CurrentPitchVelocity = 0.0f;

	float SmoothTime = 0.2f;

	bool bIsLoaded;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cannon")
	float FirePower = 3000.0f;

	ACannonBall* CurrentCannonBallRef;
};
