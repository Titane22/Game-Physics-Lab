// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PirateCharacter.h"
#include "Weapons/PirateCannon.h"
#include "Components/CapsuleComponent.h"

// Sets default values
APirateCharacter::APirateCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APirateCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &APirateCharacter::OnOverlapBegin);
	GetCapsuleComponent()->OnComponentEndOverlap.AddDynamic(this, &APirateCharacter::OnOverlapEnd);
}

// Called every frame
void APirateCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APirateCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APirateCharacter::Interact()
{
	if (OverlappingCannon)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Interact"));
		OverlappingCannon->ActivateCannonView(this);
	}
}

void APirateCharacter::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APirateCannon* Cannon = Cast<APirateCannon>(OtherActor);
	if (Cannon)
	{
		OverlappingCannon = Cannon;
	}
}

void APirateCharacter::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    APirateCannon* Cannon = Cast<APirateCannon>(OtherActor);
    if (Cannon && Cannon == OverlappingCannon)
    {
        // 오버랩 중인 캐논 참조 제거
        OverlappingCannon = nullptr;
    }
}
