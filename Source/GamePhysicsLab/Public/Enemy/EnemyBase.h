// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../GamePhysicsLabCharacter.h"
#include "EnemyBase.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPHYSICSLAB_API AEnemyBase : public AGamePhysicsLabCharacter
{
	GENERATED_BODY()
	
public:
	void ReceiveHit(bool bOnePunch, FName HitBoneName, FVector ImpactVector);
};
