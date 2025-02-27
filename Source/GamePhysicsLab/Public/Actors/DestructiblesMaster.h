// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h" 
#include "GameFramework/Actor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DestructiblesMaster.generated.h"

UCLASS()
class GAMEPHYSICSLAB_API ADestructiblesMaster : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADestructiblesMaster();

protected:
	// Called when the game starts or when spawned

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UGeometryCollectionComponent* Destructible;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* DustSpawnPoint;

	UPROPERTY()
	UTimelineComponent* MaterialFade;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Defaults")
	bool bBroken;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Defaults")
	UMaterialInstanceDynamic* DM;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Defaults")
	UMaterialInterface* MaterialInst;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	UParticleSystem* DustEmitterTemplate;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	USoundBase* BreakSound;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects", meta = (ClampMin = "0.1", ClampMax = "10.0"))
	float DustScale = 1.0f;

public:	
	void StartMaterialFade();

	void BreakObject(FVector HitLocation, FVector ImpulseDirection);

	void MakeDynamicMaterial();
};
