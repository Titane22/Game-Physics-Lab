// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapons/PirateCannon.h"
#include "InputActionValue.h"

// 전방 선언
class UInputMappingContext;
class UInputAction;
class ACannonBall;

#include "CannonBasedF.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPHYSICSLAB_API ACannonBasedF : public APirateCannon
{
	GENERATED_BODY()
protected:
	ACannonBasedF();
	
	virtual void BeginPlay();

public:
	void Fire();
	
	void Reload();
	
};
