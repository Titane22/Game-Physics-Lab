// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/CannonBasedF.h"
#include "Weapons/CannonBall.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraShakeBase.h"
#include "Blueprint/UserWidget.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

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

	ShakeClass = LoadClass<UCameraShakeBase>(nullptr, TEXT("/Game/Cannon/CS_CannonFire.CS_CannonFire_C"));
	if (ShakeClass)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("ShakeClass is Valid"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("ShakeClass is Null"));
	}
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

	if (USoundBase* cannonFire = LoadObject<USoundBase>(nullptr, TEXT("/Game/Audio/Cannon/CannonFire_Cue")))
	{
		UGameplayStatics::SpawnSoundAtLocation(
			this,
			cannonFire,
			spawnLocation
		);
	}
	else
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Sound Not Fire"));

	if (APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0))
	{
		if (CameraManager && ShakeClass)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Camera Shake"));
			CameraManager->StartCameraShake(ShakeClass);
		}
	}

	if (GunSmokeEffect)
	{
		if (UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAttached(
			GunSmokeEffect,
			MuzzleLocation,
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			true
		))
		{
			NiagaraComp->SetAutoDestroy(true);
			
			// 타이머를 사용하여 3초 후 제거
			FTimerHandle TimerHandle;
			GetWorldTimerManager().SetTimer(
				TimerHandle,
				FTimerDelegate::CreateLambda([NiagaraComp]()
				{
					if (NiagaraComp && !NiagaraComp->IsBeingDestroyed())
					{
						NiagaraComp->DeactivateImmediate();
						NiagaraComp->DestroyComponent();
					}
				}),
				3.0f,
				false
			);
		}
	}

	FActorSpawnParameters spawnParms;
	spawnParms.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	if (ACannonBall* cannonBall = GetWorld()->SpawnActor<ACannonBall>(
		CannonBallClass,
		spawnLocation,
		spawnRotation,
		spawnParms
	))
	{
		if (UProjectileMovementComponent* projectileMovement = cannonBall->FindComponentByClass<UProjectileMovementComponent>())
		{
			projectileMovement->Velocity = CannonBarrelComponent->GetForwardVector() * FirePower;
			// 대포알에 캐논 참조 설정
			cannonBall->OwnerCannon = this;
			cannonBall->OnSmokeTrail();
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

void ACannonBasedF::UpdateReloadProgress()
{
	CurrentReloadTime += 0.05f;

	ReloadProgress = FMath::Clamp(1.0f - (CurrentReloadTime / ReloadTime), 0.0f, 1.0f);
}

void ACannonBasedF::Reload()
{
	if (bIsLoaded)
		return;

	if (GetWorldTimerManager().IsTimerActive(ReloadTimerHandle))
		return;
	
	CurrentReloadTime = 0.0f;
	ReloadProgress = 1.0f;
	
	if (ReloadWidgetClass && !ReloadWidget)
	{
		if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			ReloadWidget = CreateWidget<UUserWidget>(PC, ReloadWidgetClass);
			if (ReloadWidget)
			{
				ReloadWidget->AddToViewport();
			}
		}
	}

	GetWorldTimerManager().SetTimer(
		WidgetTimerHandle,
		this,
		&ACannonBasedF::UpdateReloadProgress,
		0.05f,
		true
	);

	GetWorldTimerManager().SetTimer(
		ReloadTimerHandle,
		[this]()
		{
			bIsLoaded = true;
			ReloadProgress = 0.0f;

			GetWorldTimerManager().ClearTimer(WidgetTimerHandle);

			if (ReloadWidget)
			{
				ReloadWidget->RemoveFromParent();
				ReloadWidget = nullptr;
			}
		},
		ReloadTime,
		false
	);
}