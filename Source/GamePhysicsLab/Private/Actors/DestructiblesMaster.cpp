// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/DestructiblesMaster.h"

// Sets default values
ADestructiblesMaster::ADestructiblesMaster()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Destructible = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("Destructible"));
	DustSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("DustSpawnPoint"));

	RootComponent = Destructible;
	DustSpawnPoint->SetupAttachment(RootComponent);
}

void ADestructiblesMaster::StartMaterialFade()
{
}

void ADestructiblesMaster::BreakObject(FVector HitLocation, FVector ImpulseDirection)
{
	if (bBroken)
		return;

	bBroken = true;

	float ImpulseStrength = 1000.0f;
	
	Destructible->AddImpulseAtLocation(ImpulseDirection * ImpulseStrength, HitLocation);

	MakeDynamicMaterial();
	StartMaterialFade();

	// 사운드 재생
	if (BreakSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			BreakSound,
			GetActorLocation(),
			1.0f,
			0.7f,
			0.0f
		);
	}

	// 먼지 이펙트 스폰
	if (DustEmitterTemplate)
	{
		FVector DustLocation = DustSpawnPoint->GetComponentLocation();
		FRotator DustRotation = FRotator::ZeroRotator;
		FVector ScaleVector = FVector(DustScale);
		
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			DustEmitterTemplate,
			DustLocation,
			DustRotation,
			ScaleVector
		);
	}

	Destructible->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
}

void ADestructiblesMaster::MakeDynamicMaterial()
{
	if (!Destructible || !MaterialInst)
	{
		UE_LOG(LogTemp, Warning, TEXT("ADestructiblesMaster::MakeDynamicMaterial() Is NULL"));
		return;
	}
	DM = Destructible->CreateDynamicMaterialInstance(0, MaterialInst);

	Destructible->SetMaterial(1, DM);
}
