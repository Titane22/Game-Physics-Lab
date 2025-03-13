// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "AttachableSpawnVolume.generated.h"

class AAttachableBase;

UCLASS()
class GAMEPHYSICSLAB_API AAttachableSpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAttachableSpawnVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Spawning")
	UBoxComponent* SpawnVolume;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	TArray<TSubclassOf<AAttachableBase>> AttachableClassesToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (ClampMin = "0", UIMin = "0"))
	int32 NumberToSpawn = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float MinSpawnDistance = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Spawning")
	bool bSpawnOnBeginPlay = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	TArray<AAttachableBase*> SpawnedActors;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	FVector GetRandomPointInVolume();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void SpawnAttachables();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	AAttachableBase* SpawnSingleAttachable();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	void ClearSpawnedAttachables();

	UFUNCTION(BlueprintCallable, Category = "Spawning")
	bool IsLocationValid(const FVector& Location);
};
