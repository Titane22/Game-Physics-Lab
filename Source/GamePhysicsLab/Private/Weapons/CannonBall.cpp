// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/CannonBall.h"
#include "Camera/CameraComponent.h"
#include "Weapons/PirateCannon.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

// Sets default values
ACannonBall::ACannonBall()
{
	DefaultComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	CannonBallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Cannon Ball"));
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	CannonBallCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("CannonBall View"));
 
	RootComponent = DefaultComponent;
	CannonBallMesh->SetupAttachment(RootComponent);
	CannonBallCamera->SetupAttachment(RootComponent);

	CannonBallMesh->SetNotifyRigidBodyCollision(true);
	CannonBallMesh->OnComponentHit.AddDynamic(this, &ACannonBall::OnHit);

	CannonBallCamera->SetRelativeLocation(FVector(0.0f, 0.0f, 50.0f));
	CannonBallCamera->bUsePawnControlRotation = false;

	ProjectileMovement->bSimulationEnabled = true;
	ProjectileMovement->UpdatedComponent = CannonBallMesh;
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->MaxSpeed = 3000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;
	ProjectileMovement->Bounciness = 0.3f;
	ProjectileMovement->ProjectileGravityScale = 1.0f;


	static ConstructorHelpers::FObjectFinder<UStaticMesh> BallAsset(TEXT("/Game/Cannon/Cannonball/SM_CannonBall.SM_CannonBall"));
	if (BallAsset.Succeeded())
	{
		CannonBallMesh->SetStaticMesh(BallAsset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> ExplosionEffectAsset(TEXT("/Game/Effects/Particles/Explosion/NS_Grenade_Explosion.NS_Grenade_Explosion"));
	if (ExplosionEffectAsset.Succeeded())
	{
		ExplosionEffect = ExplosionEffectAsset.Object;
	}
}

void ACannonBall::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (ExplosionEffect)
	{
		if (UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ExplosionEffect,
			Hit.ImpactPoint,
			GetActorRotation()
		))
		{
			//NiagaraComp->Add
		}
	}

	if (ImpactDecalMat)
	{
		FRotator decalRotation = Hit.ImpactNormal.Rotation();
		decalRotation.Pitch += 90.0f;

		FVector decalSize = FVector(20.0f, 20.0f, 20.0f);
		if (UDecalComponent* DecalComp = UGameplayStatics::SpawnDecalAtLocation(
			GetWorld(),
			ImpactDecalMat,
			decalSize,
			Hit.ImpactPoint,
			decalRotation,
			10.0f
		))
		{

		}
	}

	if (USoundBase* explosionSound = LoadObject<USoundBase>(nullptr, TEXT("/Game/Audio/Cannon/SW_Explosion_Cue")))
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Explosion"));
		UGameplayStatics::SpawnSoundAtLocation(
			this,
			explosionSound,
			Hit.ImpactPoint
		);
	}

	if (OwnerCannon)
	{
		OwnerCannon->OnCannonBallHit();
	}
	OnCannonBallHit.Broadcast();

	ProjectileMovement->Velocity = FVector::ZeroVector;
	ProjectileMovement->bSimulationEnabled = false;

	FTimerHandle destroyTimerHandle;
	GetWorldTimerManager().SetTimer(
		destroyTimerHandle,
		[this]() {Destroy(); },
		1.0f,
		false
	);
}

void ACannonBall::OnSmokeTrail()
{
	if (SmokeTrailEffect)
	{
		if (UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
			SmokeTrailEffect,
			CannonBallMesh,
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true
		))
		{
			//NiagaraComp->Add
		}
	}
}

// Called when the game starts or when spawned
void ACannonBall::BeginPlay()
{
	Super::BeginPlay();
}


