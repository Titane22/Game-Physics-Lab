// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/SpringArmComponent.h"
#include "CannonBall.generated.h"

class UProjectileMovementComponent;
class UNiagaraSystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCannonBallHit);

UCLASS()
class GAMEPHYSICSLAB_API ACannonBall : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ACannonBall();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCannonBallHit OnCannonBallHit;

	UPROPERTY()
	class APirateCannon* OwnerCannon;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USceneComponent* DefaultComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* CannonBallMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	class UCameraComponent* CannonBallCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	UNiagaraSystem* ExplosionEffect;
};
