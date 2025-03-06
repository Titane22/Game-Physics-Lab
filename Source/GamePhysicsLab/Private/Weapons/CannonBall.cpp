// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/CannonBall.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
ACannonBall::ACannonBall()
{
	DefaultComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	CannonBallComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cannon Ball"));
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	RootComponent = DefaultComponent;
	CannonBallComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BallAsset(TEXT("/Game/Cannon/Cannonball/SM_CannonBall.SM_CannonBall"));
	if (BallAsset.Succeeded())
	{
		CannonBallComponent->SetStaticMesh(BallAsset.Object);
	}

}

// Called when the game starts or when spawned
void ACannonBall::BeginPlay()
{
	Super::BeginPlay();

}


