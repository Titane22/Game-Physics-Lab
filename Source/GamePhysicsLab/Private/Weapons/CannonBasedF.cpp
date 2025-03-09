// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/CannonBasedF.h"
#include "Weapons/CannonBall.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/ProjectileMovementComponent.h"

ACannonBasedF::ACannonBasedF()
	:Super()
{
	PrimaryActorTick.bCanEverTick = true;
	
	bIsLoaded = true;
	FirePower = 3000.0f;
}

void ACannonBasedF::BeginPlay()
{
	Super::BeginPlay();
}

void ACannonBasedF::Fire()
{
	if (!bIsLoaded)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Cannon is not loaded!"));
		return;
	}
	
	if (!CannonBallClass)
		return;

	FVector spawnLocation = MuzzleLocation->GetComponentLocation();
	FRotator spawnRotation = CannonBarrelComponent->GetComponentRotation();

	FActorSpawnParameters spawnParms;
	spawnParms.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	if (ACannonBall* cannonBall = GetWorld()->SpawnActor<ACannonBall>(
		CannonBallClass,
		spawnLocation,
		spawnRotation,
		spawnParms
	))
	{
		// 프로젝타일 무브먼트 컴포넌트 설정
		if (UProjectileMovementComponent* projectileMovement = cannonBall->FindComponentByClass<UProjectileMovementComponent>())
		{
			// 발사 방향 설정
			projectileMovement->Velocity = CannonBarrelComponent->GetForwardVector() * FirePower;
			// 대포알에 캐논 참조 설정
			cannonBall->OwnerCannon = this;
			
			cannonBall->OnCannonBallHit.AddDynamic(this, &ACannonBasedF::OnCannonBallHit);

			CurrentCannonBallRef = cannonBall;
			
			bIsLoaded = false;
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Cannon fired!"));
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("ACannonBasedF::Fire() is Failed to FindComponentByClass ProjectileMovement"));
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("ACannonBasedF::Fire() is Failed to Spawn Cannon Ball"));
	}
}

void ACannonBasedF::Reload()
{
	if (bIsLoaded)
		return;

	FTimerHandle timerHandler;
	GetWorldTimerManager().SetTimer(
		timerHandler,
		[this]()
		{
			bIsLoaded = true;

		},
		1.0f,
		false
	);
}