// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/PirateCannon.h"

// Sets default values
APirateCannon::APirateCannon()
{
	DefaultComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	CannonBaseComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Base"));
	CannonBarrelComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Barrel"));
	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	
	RootComponent = DefaultComponent;
	CannonBaseComponent->SetupAttachment(RootComponent);
	CannonBarrelComponent->SetupAttachment(CannonBaseComponent);
	MuzzleLocation->SetupAttachment(CannonBarrelComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BaseAsset(TEXT("/Game/Cannon/SM_CannonBase.SM_CannonBase"));
	if (BaseAsset.Succeeded())
	{
		CannonBaseComponent->SetStaticMesh(BaseAsset.Object);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> BarrelAsset(TEXT("/Game/Cannon/SM_CannonBarrel.SM_CannonBarrel"));
	if (BarrelAsset.Succeeded())
	{
		CannonBarrelComponent->SetStaticMesh(BarrelAsset.Object);
	}
}

// Called when the game starts or when spawned
void APirateCannon::BeginPlay()
{
	Super::BeginPlay();
	
}

