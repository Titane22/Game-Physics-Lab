// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GamePhysicsLab/GamePhysicsLabCharacter.h"
#include "PirateCharacter.generated.h"

UCLASS()
class GAMEPHYSICSLAB_API APirateCharacter : public AGamePhysicsLabCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APirateCharacter();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void Interact() override;

	UPROPERTY()
	class APirateCannon* OverlappingCannon;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
                   bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
					 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};